#!/usr/bin/env python3
"""
WHCA* Python Node — drop-in replacement for mapf_base_node when using WHCA*.

Subscribes to the same topics, publishes the same messages.
The algorithm is implemented in pure Python so you can read, debug, and modify it.

Usage:
  ros2 run mapf_base whca_node.py --ros-args -p agent_num:=4 -p window_size:=32
"""
import heapq
from dataclasses import dataclass, field
from typing import Optional

import numpy as np
import rclpy
from geometry_msgs.msg import PoseStamped
from mapf_msgs.msg import GlobalPlan, Goal, SinglePlan
from nav_msgs.msg import OccupancyGrid, Path
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from std_msgs.msg import Bool
from tf2_ros import Buffer, TransformListener
from visualization_msgs.msg import Marker, MarkerArray


# =============================================================================
# WHCA* Algorithm (pure Python)
# =============================================================================


@dataclass(frozen=True)
class State:
    """A position in space-time: (x, y) at time t."""
    x: int
    y: int
    t: int


@dataclass(frozen=True)
class Edge:
    """A directed edge: moving from (x1,y1) to (x2,y2) at time t."""
    x1: int
    y1: int
    x2: int
    y2: int
    t: int


class ReservationTable:
    """
    Tracks which cells are reserved at which time steps.

    Two types of reservations:
      - Vertex: cell (x,y) is occupied at time t
      - Edge: transition from (x1,y1)->(x2,y2) at time t is taken
              (prevents two robots from swapping positions)
    """

    def __init__(self) -> None:
        self.vertices: set[tuple[int, int, int]] = set()  # (x, y, t)
        self.edges: set[tuple[int, int, int, int, int]] = set()  # (x1, y1, x2, y2, t)

    def reserve_vertex(self, x: int, y: int, t: int) -> None:
        self.vertices.add((x, y, t))

    def reserve_edge(self, x1: int, y1: int, x2: int, y2: int, t: int) -> None:
        self.edges.add((x1, y1, x2, y2, t))

    def is_vertex_reserved(self, x: int, y: int, t: int) -> bool:
        return (x, y, t) in self.vertices

    def is_edge_reserved(self, x1: int, y1: int, x2: int, y2: int, t: int) -> bool:
        return (x1, y1, x2, y2, t) in self.edges

    def __repr__(self) -> str:
        return f"ReservationTable(vertices={len(self.vertices)}, edges={len(self.edges)})"


# 5 possible actions: wait, up, down, left, right
ACTIONS = [(0, 0), (0, 1), (0, -1), (-1, 0), (1, 0)]


def heuristic(x: int, y: int, gx: int, gy: int) -> int:
    """Manhattan distance heuristic."""
    return abs(x - gx) + abs(y - gy)


def windowed_astar(
    start: State,
    goal_x: int,
    goal_y: int,
    window_size: int,
    grid: np.ndarray,
    reservation_table: ReservationTable,
) -> Optional[list[State]]:
    """
    A* search in space-time for a single robot.

    The search space is 3D: (x, y, t). Each step increments t by 1.
    A state is valid if:
      - It's within the grid bounds
      - It's not a wall (grid cell is free)
      - It's not reserved by a higher-priority robot at that time

    Returns a list of States from start to goal (or end of window),
    or None if no path found.
    """
    dimx, dimy = grid.shape

    # Priority queue: (f_cost, tie_breaker, state)
    counter = 0
    open_set: list[tuple[int, int, State]] = []
    g_cost = start.t
    f_cost = g_cost + heuristic(start.x, start.y, goal_x, goal_y)
    heapq.heappush(open_set, (f_cost, counter, start))

    came_from: dict[State, State] = {}
    g_scores: dict[State, int] = {start: start.t}

    while open_set:
        _, _, current = heapq.heappop(open_set)

        # Reached the goal?
        if current.x == goal_x and current.y == goal_y:
            return _reconstruct_path(came_from, current, start)

        # Reached end of window? Return best path so far.
        if current.t >= window_size:
            return _reconstruct_path(came_from, current, start)

        # Expand neighbors (5 actions)
        for dx, dy in ACTIONS:
            nx, ny, nt = current.x + dx, current.y + dy, current.t + 1

            # Bounds check
            if nx < 0 or nx >= dimx or ny < 0 or ny >= dimy:
                continue

            # Wall check (grid is 1=obstacle, 0=free)
            if grid[nx, ny] == 1:
                continue

            # Vertex conflict: another robot is at (nx,ny) at time nt
            if reservation_table.is_vertex_reserved(nx, ny, nt):
                continue

            # Edge conflict: another robot moves FROM (nx,ny) TO (current.x,current.y)
            # at the same time — i.e., they would swap
            if reservation_table.is_edge_reserved(nx, ny, current.x, current.y, current.t):
                continue

            neighbor = State(nx, ny, nt)
            tentative_g = nt

            if tentative_g < g_scores.get(neighbor, float("inf")):
                came_from[neighbor] = current
                g_scores[neighbor] = tentative_g
                f = tentative_g + heuristic(nx, ny, goal_x, goal_y)
                counter += 1
                heapq.heappush(open_set, (f, counter, neighbor))

    return None  # No path found


def _reconstruct_path(
    came_from: dict[State, State], current: State, start: State
) -> list[State]:
    """Walk back through came_from to build the path."""
    path = [current]
    while current in came_from:
        current = came_from[current]
        path.append(current)
    path.reverse()
    return path


def _plan_one_window(
    starts: list[tuple[int, int]],
    goals: list[tuple[int, int]],
    grid: np.ndarray,
    window_size: int,
    arrived: list[bool],
) -> Optional[list[list[State]]]:
    """
    Plan one window of WHCA* for all agents.

    Agents that have already arrived stay in place.
    Returns per-agent paths for this window, or None on failure.
    """
    num_agents = len(starts)
    reservation_table = ReservationTable()

    # Pre-reserve goals of agents that haven't arrived yet, to prevent
    # earlier-priority robots from blocking later robots' destinations.
    # Also reserve positions of already-arrived robots for all time steps.
    # Exception: don't reserve at t=0 if another robot is starting there
    # (it needs to be able to leave that cell).
    start_positions = set(starts)
    goal_reservations: list[set[tuple[int, int, int]]] = []
    for i in range(num_agents):
        reserved = set()
        gx, gy = goals[i]
        for t in range(window_size + 1):
            # Skip t=0 reservation if a robot starts at this goal cell
            # (that robot needs to move away, not be blocked at its start)
            if t == 0 and (gx, gy) in start_positions and not arrived[i]:
                continue
            reservation_table.reserve_vertex(gx, gy, t)
            reserved.add((gx, gy, t))
        goal_reservations.append(reserved)

    paths: list[list[State]] = []

    for i in range(num_agents):
        goal_x, goal_y = goals[i]

        if arrived[i]:
            # Already at goal — stay in place (just one state for path output,
            # already reserved by goal_reservations above)
            path = [State(goal_x, goal_y, 0)]
            paths.append(path)
            continue

        start = State(starts[i][0], starts[i][1], 0)

        # Temporarily remove THIS robot's goal reservations so it can reach its goal.
        # Also remove reservations at t=0 for this robot's start position
        # (in case its start overlaps with another robot's goal).
        for entry in goal_reservations[i]:
            reservation_table.vertices.discard(entry)
        # If this robot starts on someone else's goal, temporarily unblock t=0
        # so the A* start state is valid
        start_on_other_goal = (start.x, start.y, 0) in reservation_table.vertices
        if start_on_other_goal:
            reservation_table.vertices.discard((start.x, start.y, 0))

        path = windowed_astar(
            start, goal_x, goal_y, window_size, grid, reservation_table
        )

        if path is None:
            return None

        # Reserve the planned path
        for state in path:
            reservation_table.reserve_vertex(state.x, state.y, state.t)

        # Reserve edges
        for j in range(len(path) - 1):
            s1, s2 = path[j], path[j + 1]
            reservation_table.reserve_edge(s1.x, s1.y, s2.x, s2.y, s1.t)

        # Hold final position until end of window
        last = path[-1]
        for t in range(last.t + 1, window_size + 1):
            reservation_table.reserve_vertex(last.x, last.y, t)

        # Re-add goal reservations and the start-overlap reservation
        for entry in goal_reservations[i]:
            reservation_table.vertices.add(entry)
        if start_on_other_goal:
            reservation_table.vertices.add((start.x, start.y, 0))

        paths.append(path)

    return paths


def whca_star(
    starts: list[tuple[int, int]],
    goals: list[tuple[int, int]],
    grid: np.ndarray,
    window_size: int,
    logger=None,
    max_iterations: int = 20,
) -> Optional[list[list[State]]]:
    """
    Iterative WHCA*: repeatedly plans W steps, advances, replans until done.

    This is the true WHCA* algorithm:
      1. Plan a window of W steps for all agents
      2. "Execute" the window (advance all agents to their positions at t=W)
      3. Agents that reached their goal stop permanently
      4. Replan from new positions with a fresh reservation table
      5. Repeat until all agents have reached their goals

    The iterative approach naturally handles the goal-occupation problem:
    arrived robots are in the reservation table from t=0 of each new window.

    Returns concatenated full paths for all agents, or None on failure.
    """
    num_agents = len(starts)
    current_positions = list(starts)
    arrived = [starts[i] == goals[i] for i in range(num_agents)]
    arrival_times = [0 if arrived[i] else -1 for i in range(num_agents)]
    full_paths: list[list[State]] = [[] for _ in range(num_agents)]
    total_time_offset = 0

    for iteration in range(max_iterations):
        if all(arrived):
            break

        if logger:
            active = sum(1 for a in arrived if not a)
            logger.info(
                f"--- Iteration {iteration + 1}: "
                f"{active} active agents, "
                f"time_offset={total_time_offset} ---"
            )

        # Plan one window from current positions
        window_paths = _plan_one_window(
            current_positions, goals, grid, window_size, arrived
        )

        if window_paths is None:
            if logger:
                logger.error(
                    f"Planning failed at iteration {iteration + 1}"
                )
            return None

        # Append this window's paths to the full paths (with time offset)
        for i in range(num_agents):
            window_path = window_paths[i]

            if arrived[i]:
                # Already arrived — don't append more states, the trimming
                # step will pad with hold-in-place later
                continue

            if iteration == 0:
                # First iteration: include all states
                for state in window_path:
                    full_paths[i].append(
                        State(state.x, state.y, state.t + total_time_offset)
                    )
            else:
                # Subsequent iterations: skip t=0 (it's the same as previous
                # window's last state)
                for state in window_path[1:]:
                    full_paths[i].append(
                        State(state.x, state.y, state.t + total_time_offset)
                    )

        # Advance: move all agents to their positions at end of window
        total_time_offset += window_size

        for i in range(num_agents):
            if arrived[i]:
                continue
            last = window_paths[i][-1]
            current_positions[i] = (last.x, last.y)
            # Check if this agent reached its goal
            if (last.x, last.y) == goals[i]:
                arrived[i] = True
                # Record actual arrival time (offset + time within window)
                arrival_times[i] = total_time_offset - window_size + last.t
                if logger:
                    logger.info(
                        f"  Agent {i} ARRIVED at goal "
                        f"({last.x},{last.y}) at t={arrival_times[i]}"
                    )

    if not all(arrived):
        if logger:
            not_arrived = [i for i, a in enumerate(arrived) if not a]
            logger.error(
                f"Max iterations ({max_iterations}) reached. "
                f"Agents not arrived: {not_arrived}"
            )
        return None

    if logger:
        logger.info(
            f"All agents arrived! Total time steps: {total_time_offset}"
        )

    # Trim trailing hold-in-place states for cleaner output.
    # Use recorded arrival_times (not path search) to find when last robot arrived.
    max_arrival = max(arrival_times)

    # Keep all paths up to max_arrival so the animator shows all robots
    # moving until everyone has arrived
    trimmed_paths: list[list[State]] = []
    for i in range(num_agents):
        trimmed = []
        for state in full_paths[i]:
            trimmed.append(state)
            if state.t >= max_arrival:
                break
        # If path is shorter than max_arrival, pad with last position
        if trimmed and trimmed[-1].t < max_arrival:
            last = trimmed[-1]
            for t in range(last.t + 1, max_arrival + 1):
                trimmed.append(State(last.x, last.y, t))
        trimmed_paths.append(trimmed)

    # Verify no conflicts
    if logger:
        max_t = max(p[-1].t for p in trimmed_paths)
        extended: list[list[tuple[int, int]]] = []
        for path in trimmed_paths:
            positions = [(s.x, s.y) for s in path]
            last = positions[-1]
            while len(positions) <= max_t:
                positions.append(last)
            extended.append(positions)

        conflicts = 0
        for t in range(max_t + 1):
            occupied: dict[tuple[int, int], int] = {}
            for agent_id, positions in enumerate(extended):
                pos = positions[t]
                if pos in occupied:
                    logger.warn(
                        f"CONFLICT at t={t}: agent {occupied[pos]} and "
                        f"agent {agent_id} both at ({pos[0]},{pos[1]})"
                    )
                    conflicts += 1
                occupied[pos] = agent_id
        if conflicts == 0:
            logger.info("Verified: no vertex conflicts in solution")
        else:
            logger.error(f"Found {conflicts} vertex conflicts!")

    return trimmed_paths


# =============================================================================
# ROS2 Node
# =============================================================================


class WHCANode(Node):
    """
    Python WHCA* planner node with dynamic obstacle support.

    Replaces the C++ mapf_base_node + WHCA plugin with a single Python node.
    Same topic interface — works with existing goal_transformer, plan_animator, etc.

    Dynamic obstacle support:
      - Subscribes to costmap updates (OccupancyGrid on /mapf/dynamic_obstacles)
      - When obstacles change, triggers a replan from current robot positions
      - Committed steps (already being executed) are not affected
      - Only tentative future steps are replanned with updated grid
    """

    def __init__(self) -> None:
        super().__init__("whca_node")

        # Parameters
        self.declare_parameter("agent_num", 8)
        self.declare_parameter("window_size", 48)
        self.declare_parameter("global_frame_id", "map")
        self.declare_parameter("planner_time_tolerance", 60.0)
        self.declare_parameter("replan_interval", 0.0)  # seconds, 0=disabled
        self.declare_parameter("commit_steps", 5)  # steps locked during replan

        self.agent_num = self.get_parameter("agent_num").value
        self.window_size = self.get_parameter("window_size").value
        self.global_frame_id = self.get_parameter("global_frame_id").value
        self.replan_interval = self.get_parameter("replan_interval").value
        self.commit_steps = self.get_parameter("commit_steps").value

        self.get_logger().info(
            f"WHCA* Python node: {self.agent_num} agents, "
            f"window_size={self.window_size}, "
            f"replan_interval={self.replan_interval}s, "
            f"commit_steps={self.commit_steps}"
        )

        # State
        self.static_grid: Optional[np.ndarray] = None  # base map (never changes)
        self.grid: Optional[np.ndarray] = None  # static + dynamic merged
        self.resolution: float = 1.0
        self.origin_x: float = 0.0
        self.origin_y: float = 0.0
        self.goals_grid: Optional[list[tuple[int, int]]] = None
        self.planning_active: bool = False
        self.grid_changed: bool = False
        self.current_plan_start_time: Optional[float] = None

        # TF for getting robot positions
        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

        # Declare base_frame_id parameters
        self.base_frames: list[str] = []
        for i in range(self.agent_num):
            param_name = f"base_frame_id.agent_{i}"
            self.declare_parameter(param_name, f"robot_{i}/base_footprint")
            self.base_frames.append(self.get_parameter(param_name).value)

        # Declare plan_topic parameters
        self.plan_topics: list[str] = []
        self.plan_pubs = []
        for i in range(self.agent_num):
            param_name = f"plan_topic.agent_{i}"
            self.declare_parameter(param_name, f"robot_{i}/plan")
            topic = self.get_parameter(param_name).value
            self.plan_topics.append(topic)
            self.plan_pubs.append(
                self.create_publisher(Path, topic, 1)
            )

        # Subscribe to static map (transient local to get latched map)
        map_qos = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            durability=DurabilityPolicy.TRANSIENT_LOCAL,
            depth=1,
        )
        self.sub_map = self.create_subscription(
            OccupancyGrid, "map", self.map_callback, map_qos
        )

        # Subscribe to dynamic obstacle updates (volatile — continuous updates)
        self.sub_dynamic = self.create_subscription(
            OccupancyGrid, "dynamic_obstacles", self.dynamic_obstacle_callback, 1
        )

        # Subscribe to goal (from goal_transformer)
        self.sub_goal = self.create_subscription(
            Goal, "mapf_goal", self.goal_callback, 1
        )

        # Publish global plan
        self.pub_global_plan = self.create_publisher(GlobalPlan, "global_plan", 1)

        # Publish goal markers for RViz
        self.pub_goal_markers = self.create_publisher(MarkerArray, "goal_markers", 1)

        # Publish dynamic obstacle overlay for RViz
        self.pub_obstacle_markers = self.create_publisher(
            MarkerArray, "dynamic_obstacle_markers", 1
        )

        # Robot colors: red, blue, green, purple, orange, cyan, yellow, magenta
        self.robot_colors = [
            (1.0, 0.0, 0.0), (0.0, 0.0, 1.0),
            (0.0, 0.8, 0.0), (0.6, 0.0, 0.8),
            (1.0, 0.5, 0.0), (0.0, 0.8, 0.8),
            (0.9, 0.9, 0.0), (0.9, 0.0, 0.5),
        ]

        # Replan timer (if enabled)
        if self.replan_interval > 0:
            self.replan_timer = self.create_timer(
                self.replan_interval, self.replan_callback
            )
            self.get_logger().info(
                f"Periodic replanning enabled every {self.replan_interval}s"
            )

        self.get_logger().info("WHCA* Python node ready. Waiting for map and goals...")

    def map_callback(self, msg: OccupancyGrid) -> None:
        """Convert OccupancyGrid to a binary numpy grid."""
        w, h = msg.info.width, msg.info.height
        self.resolution = msg.info.resolution
        self.origin_x = msg.info.origin.position.x
        self.origin_y = msg.info.origin.position.y

        # OccupancyGrid data: 0=free, 100=occupied, -1=unknown
        raw = np.array(msg.data, dtype=np.int8).reshape((h, w))
        # Convert to binary: 1=obstacle, 0=free
        # Treat unknown (-1) as obstacle for safety
        self.static_grid = np.where(raw > 50, 1, np.where(raw < 0, 1, 0)).astype(np.int8)
        # Transpose so grid[x, y] matches our coordinate system
        self.static_grid = self.static_grid.T
        # Initialize planning grid as copy of static
        self.grid = self.static_grid.copy()

        self.get_logger().info(
            f"Map received: {w}x{h}, resolution={self.resolution}m/cell, "
            f"obstacles={np.sum(self.grid == 1)}"
        )

    def dynamic_obstacle_callback(self, msg: OccupancyGrid) -> None:
        """
        Update grid with dynamic obstacles.

        Merges dynamic obstacles onto the static map. Any cell marked as
        occupied (>50) in this message becomes blocked. Previously dynamic
        obstacles are cleared — this message represents the CURRENT state
        of all dynamic obstacles.

        Publish this topic to simulate moving obstacles:
          ros2 topic pub /mapf/dynamic_obstacles nav_msgs/OccupancyGrid ...
        """
        if self.static_grid is None:
            return

        w, h = msg.info.width, msg.info.height
        if w != self.static_grid.shape[0] or h != self.static_grid.shape[1]:
            self.get_logger().warn(
                f"Dynamic obstacle grid size ({w}x{h}) doesn't match "
                f"base map ({self.static_grid.shape[0]}x{self.static_grid.shape[1]}). "
                f"Ignoring."
            )
            return

        raw = np.array(msg.data, dtype=np.int8).reshape((h, w))
        dynamic_obstacles = np.where(raw > 50, 1, 0).astype(np.int8).T

        # Merge onto STATIC base (so removed dynamic obstacles disappear)
        new_grid = np.maximum(self.static_grid, dynamic_obstacles)

        if not np.array_equal(new_grid, self.grid):
            self.grid = new_grid
            self.grid_changed = True
            num_dynamic = int(np.sum(dynamic_obstacles == 1))
            self.get_logger().info(
                f"Dynamic obstacles updated: {num_dynamic} dynamic + "
                f"{int(np.sum(self.static_grid == 1))} static = "
                f"{int(np.sum(new_grid == 1))} total blocked cells"
            )

            # Publish markers for dynamic obstacles in RViz
            self._publish_dynamic_obstacle_markers(dynamic_obstacles)

    def _publish_dynamic_obstacle_markers(self, dynamic_grid: np.ndarray) -> None:
        """Visualize dynamic obstacles as red cubes in RViz."""
        markers = MarkerArray()
        now = self.get_clock().now().to_msg()

        # Delete old markers
        delete_marker = Marker()
        delete_marker.header.frame_id = self.global_frame_id
        delete_marker.header.stamp = now
        delete_marker.ns = "dynamic_obstacles"
        delete_marker.action = Marker.DELETEALL
        markers.markers.append(delete_marker)

        marker_id = 0
        for x in range(dynamic_grid.shape[0]):
            for y in range(dynamic_grid.shape[1]):
                if dynamic_grid[x, y] == 1:
                    m = Marker()
                    m.header.frame_id = self.global_frame_id
                    m.header.stamp = now
                    m.ns = "dynamic_obstacles"
                    m.id = marker_id
                    m.type = Marker.CUBE
                    m.action = Marker.ADD
                    wx, wy = self.map_to_world(x, y)
                    m.pose.position.x = wx
                    m.pose.position.y = wy
                    m.pose.position.z = 0.5
                    m.pose.orientation.w = 1.0
                    m.scale.x = self.resolution * 0.9
                    m.scale.y = self.resolution * 0.9
                    m.scale.z = 1.0
                    m.color.r = 1.0
                    m.color.g = 0.2
                    m.color.b = 0.2
                    m.color.a = 0.7
                    markers.markers.append(m)
                    marker_id += 1

        self.pub_obstacle_markers.publish(markers)

    def replan_callback(self) -> None:
        """
        Periodic replanning triggered by timer or grid changes.

        Reads current robot positions from TF, replans from there
        with updated grid. Only replans if:
          - Goals are active (planning was triggered)
          - Grid has changed OR replan interval elapsed
        """
        if self.goals_grid is None or not self.planning_active:
            return

        if not self.grid_changed:
            return

        self.grid_changed = False
        self.get_logger().info("REPLAN triggered (dynamic obstacles changed)")
        self._do_planning()

    def goal_callback(self, msg: Goal) -> None:
        """Receive goals and trigger initial planning."""
        self.get_logger().info(f"Received MAPF goal with {len(msg.goal.poses)} poses")

        if self.grid is None:
            self.get_logger().error("No map received yet!")
            return

        # Extract goal positions in grid coordinates
        self.goals_grid = []
        for pose in msg.goal.poses:
            gx, gy = self.world_to_map(
                pose.pose.position.x, pose.pose.position.y
            )
            self.goals_grid.append((gx, gy))

        # Publish goal markers at the actual cell centers the planner targets
        self.publish_goal_markers(self.goals_grid)

        self.planning_active = True
        self._do_planning()

    def _do_planning(self) -> None:
        """Run WHCA* from current robot positions to goals."""
        import time

        if self.goals_grid is None or self.grid is None:
            return

        # Get robot start positions from TF
        starts_grid = []
        for i in range(self.agent_num):
            wx, wy = self.get_robot_position(i)
            if wx is None:
                self.get_logger().error(f"Cannot get position for agent {i}")
                return
            sx, sy = self.world_to_map(wx, wy)
            starts_grid.append((sx, sy))

        self.get_logger().info("=" * 50)
        self.get_logger().info("WHCA* PLANNING START")
        self.get_logger().info("=" * 50)
        for i, ((sx, sy), (gx, gy)) in enumerate(zip(starts_grid, self.goals_grid)):
            self.get_logger().info(
                f"  Agent {i}: ({sx},{sy}) -> ({gx},{gy})"
            )

        t0 = time.time()
        paths = whca_star(
            starts_grid, self.goals_grid, self.grid, self.window_size,
            logger=self.get_logger(),
        )
        elapsed = time.time() - t0

        if paths is None:
            self.get_logger().error(f"WHCA* FAILED after {elapsed:.2f}s")
            return

        self.get_logger().info(f"WHCA* SUCCESS in {elapsed:.2f}s")
        self.current_plan_start_time = time.time()

        # Convert to ROS messages and publish
        global_plan = self.paths_to_global_plan(paths, self.goals_grid)
        self.pub_global_plan.publish(global_plan)

        # Also publish individual paths for RViz
        for i, single_plan in enumerate(global_plan.global_plan):
            self.plan_pubs[i].publish(single_plan.plan)

        self.get_logger().info(
            f"Published plan: makespan={global_plan.makespan}"
        )

    def paths_to_global_plan(
        self, paths: list[list[State]], goals_grid: list[tuple[int, int]]
    ) -> GlobalPlan:
        """Convert algorithm output to mapf_msgs/GlobalPlan."""
        plan = GlobalPlan()
        plan.makespan = max(len(p) for p in paths)

        for i, path in enumerate(paths):
            single = SinglePlan()
            single.plan.header.frame_id = self.global_frame_id
            single.plan.header.stamp = self.get_clock().now().to_msg()

            # Skip the first state (start position) unless it's a wait-in-place plan
            start_idx = 1 if len(path) > 1 else 0

            for state in path[start_idx:]:
                pose = PoseStamped()
                pose.header.frame_id = self.global_frame_id
                pose.pose.orientation.w = 1.0
                wx, wy = self.map_to_world(state.x, state.y)
                pose.pose.position.x = wx
                pose.pose.position.y = wy
                single.plan.poses.append(pose)
                single.time_step.append(state.t)

            # Replace last pose with exact goal if agent reached it
            if path[-1].x == goals_grid[i][0] and path[-1].y == goals_grid[i][1]:
                if single.plan.poses:
                    gx, gy = goals_grid[i]
                    wx, wy = self.map_to_world(gx, gy)
                    single.plan.poses[-1].pose.position.x = wx
                    single.plan.poses[-1].pose.position.y = wy

            plan.global_plan.append(single)

        return plan

    def get_robot_position(self, agent_idx: int) -> tuple[Optional[float], Optional[float]]:
        """Get robot position from TF tree."""
        try:
            transform = self.tf_buffer.lookup_transform(
                self.global_frame_id,
                self.base_frames[agent_idx],
                rclpy.time.Time(),
            )
            x = transform.transform.translation.x
            y = transform.transform.translation.y
            return x, y
        except Exception as e:
            self.get_logger().warn(f"TF lookup failed for agent {agent_idx}: {e}")
            return None, None

    def world_to_map(self, wx: float, wy: float) -> tuple[int, int]:
        """Convert world coordinates to grid cell."""
        mx = int((wx - self.origin_x) / self.resolution)
        my = int((wy - self.origin_y) / self.resolution)
        return mx, my

    def map_to_world(self, mx: int, my: int) -> tuple[float, float]:
        """Convert grid cell to world coordinates (cell center)."""
        wx = self.origin_x + (mx + 0.5) * self.resolution
        wy = self.origin_y + (my + 0.5) * self.resolution
        return wx, wy

    def publish_goal_markers(self, goals_grid: list[tuple[int, int]]) -> None:
        """Publish color-coded goal markers (X shape + label) for each robot."""
        markers = MarkerArray()
        now = self.get_clock().now().to_msg()

        for i, (grid_x, grid_y) in enumerate(goals_grid):
            r, g, b = self.robot_colors[i % len(self.robot_colors)]
            gx, gy = self.map_to_world(grid_x, grid_y)

            # Goal ring (cylinder, flat and transparent)
            ring = Marker()
            ring.header.frame_id = self.global_frame_id
            ring.header.stamp = now
            ring.ns = "goal_rings"
            ring.id = i
            ring.type = Marker.CYLINDER
            ring.action = Marker.ADD
            ring.pose.position.x = gx
            ring.pose.position.y = gy
            ring.pose.position.z = 0.05
            ring.pose.orientation.w = 1.0
            ring.scale.x = 1.2
            ring.scale.y = 1.2
            ring.scale.z = 0.1
            ring.color.r = r
            ring.color.g = g
            ring.color.b = b
            ring.color.a = 0.4
            markers.markers.append(ring)

            # Goal X marker (mesh cross using two cubes)
            cross1 = Marker()
            cross1.header.frame_id = self.global_frame_id
            cross1.header.stamp = now
            cross1.ns = "goal_cross_1"
            cross1.id = i
            cross1.type = Marker.CUBE
            cross1.action = Marker.ADD
            cross1.pose.position.x = gx
            cross1.pose.position.y = gy
            cross1.pose.position.z = 0.15
            # Rotated 45 degrees
            cross1.pose.orientation.z = 0.3827
            cross1.pose.orientation.w = 0.9239
            cross1.scale.x = 1.0
            cross1.scale.y = 0.15
            cross1.scale.z = 0.15
            cross1.color.r = r
            cross1.color.g = g
            cross1.color.b = b
            cross1.color.a = 0.9
            markers.markers.append(cross1)

            cross2 = Marker()
            cross2.header.frame_id = self.global_frame_id
            cross2.header.stamp = now
            cross2.ns = "goal_cross_2"
            cross2.id = i
            cross2.type = Marker.CUBE
            cross2.action = Marker.ADD
            cross2.pose.position.x = gx
            cross2.pose.position.y = gy
            cross2.pose.position.z = 0.15
            # Rotated -45 degrees
            cross2.pose.orientation.z = -0.3827
            cross2.pose.orientation.w = 0.9239
            cross2.scale.x = 1.0
            cross2.scale.y = 0.15
            cross2.scale.z = 0.15
            cross2.color.r = r
            cross2.color.g = g
            cross2.color.b = b
            cross2.color.a = 0.9
            markers.markers.append(cross2)

            # Label: "G0", "G1", etc.
            label = Marker()
            label.header.frame_id = self.global_frame_id
            label.header.stamp = now
            label.ns = "goal_labels"
            label.id = i
            label.type = Marker.TEXT_VIEW_FACING
            label.action = Marker.ADD
            label.pose.position.x = gx
            label.pose.position.y = gy
            label.pose.position.z = 0.8
            label.pose.orientation.w = 1.0
            label.scale.z = 0.5
            label.color.r = r
            label.color.g = g
            label.color.b = b
            label.color.a = 1.0
            label.text = f"G{i}"
            markers.markers.append(label)

        self.pub_goal_markers.publish(markers)


def main() -> None:
    rclpy.init()
    node = WHCANode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
