# Multi-Agent Path Finding (MAPF) with WHCA*

ROS2 Humble workspace for multi-robot path planning using WHCA* (Windowed Hierarchical Cooperative A*). Robots coordinate collision-free paths through a warehouse environment with narrow aisles and shelves.

## Prerequisites

- ROS2 Humble
- nav2 stack (`nav2_costmap_2d`, `nav2_map_server`, `nav2_lifecycle_manager`, `nav2_util`)
- `tf2_ros`, `pluginlib`

## Build

```bash
cd ~/ros2_map
colcon build --symlink-install
source install/setup.bash
```

## Run the WHCA* Simulation

### Step 1: Launch the system (Terminal 1)

```bash
source install/setup.bash
ros2 launch mapf_base whca_python.launch.py
```

This starts:
- Map server (30x20 warehouse grid at 1.5m/cell)
- WHCA* Python planner node (iterative replanning)
- Plan animator (publishes TF + robot markers at 30Hz)
- Goal transformer (collects per-robot goals)

### Step 2: Open RViz (Terminal 2)

```bash
source install/setup.bash
rviz2 -d install/mapf_base/share/mapf_base/params/whca_python.rviz
```

You should see:
- The warehouse map with shelves
- 8 colored robots at two charging stations (4 bottom-left, 4 bottom-right)

### Step 3: Run the demo (Terminal 3)

```bash
source install/setup.bash
python3 mapf_ros/mapf_base/scripts/whca_demo.py
```

The demo dispatches 8 robots from two charging stations to pick locations across the warehouse. Robots from the left station must cross to the right side, and vice versa, forcing their paths to tangle in the center aisles.

## Launch Arguments

| Argument | Default | Description |
|----------|---------|-------------|
| `window_size` | 48 | WHCA* planning horizon (time steps) |
| `replan_interval` | 2.0 | Seconds between dynamic replan checks (0=disabled) |
| `map` | warehouse.yaml | Map file to use |

Example with larger window:
```bash
ros2 launch mapf_base whca_python.launch.py window_size:=64
```

## Dynamic Obstacles

The planner supports dynamic obstacles with automatic replanning. See [docs/dynamic_obstacles.md](docs/dynamic_obstacles.md) for full instructions on publishing obstacles and triggering replans.

## Map Variants

Generate different warehouse layouts:

```bash
cd mapf_ros/mapf_base/maps
python3 warehouse.py simple    # fewer shelves, wider aisles
python3 warehouse.py complex   # dense shelves, narrow aisles (default)
python3 warehouse.py maze      # maze-like corridors
```

Then launch with:
```bash
ros2 launch mapf_base whca_python.launch.py map:=warehouse_simple.yaml
```

## Stress Test

Run progressively harder scenarios to find WHCA*'s limits:

```bash
python3 mapf_ros/mapf_base/scripts/whca_stress_test.py      # all levels
python3 mapf_ros/mapf_base/scripts/whca_stress_test.py 3    # specific level
```

## Troubleshooting

**Stale processes**: Always kill old ROS processes before relaunching:
```bash
pkill -f 'ros2|mapf|rviz'
```

**Planner timeout**: Increase window size or use a simpler map layout.

**No map in RViz**: The Python launch uses `/mapf/map` with Transient Local QoS. Make sure you use the `whca_python.rviz` config.
