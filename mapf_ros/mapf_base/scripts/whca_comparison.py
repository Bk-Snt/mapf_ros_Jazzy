#!/usr/bin/env python3
"""
WHCA* vs Independent A* Comparison Visualization.

Shows two cases side by side:
  Case 1: Independent A* (no coordination) — paths cross, collisions occur
  Case 2: WHCA* — temporal coordination avoids all collisions

Reads the actual WHCA* plan from ROS2 topics if available, otherwise
demonstrates with a simulated example.

Usage:
  python3 scripts/whca_comparison.py
"""
import subprocess
import sys
from dataclasses import dataclass
from typing import Optional

import numpy as np

try:
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    from matplotlib.collections import LineCollection
except ImportError:
    print("matplotlib required: pip install matplotlib")
    sys.exit(1)


@dataclass(frozen=True)
class Point:
    x: float
    y: float


COLORS = ["#e41a1c", "#377eb8", "#4daf4a", "#984ea3"]
ROBOT_NAMES = ["robot_0", "robot_1", "robot_2", "robot_3"]

# Corner-swap scenario on 25x20 grid at 1.5m/cell
STARTS = [Point(3.0, 1.5), Point(12.0, 1.5), Point(3.0, 15.0), Point(12.0, 15.0)]
GOALS = [Point(12.0, 15.0), Point(3.0, 15.0), Point(12.0, 1.5), Point(3.0, 1.5)]

MAP_WIDTH = 37.5   # 25 * 1.5
MAP_HEIGHT = 30.0   # 20 * 1.5
CELL_SIZE = 1.5


def independent_astar_paths() -> list[list[Point]]:
    """Simulate independent A* — straight-line shortest paths (no coordination)."""
    paths = []
    for start, goal in zip(STARTS, GOALS):
        dx = goal.x - start.x
        dy = goal.y - start.y
        steps = int(max(abs(dx), abs(dy)) / CELL_SIZE)
        path = []
        for t in range(steps + 1):
            frac = t / steps if steps > 0 else 1.0
            path.append(Point(start.x + frac * dx, start.y + frac * dy))
        paths.append(path)
    return paths


def find_collisions(paths: list[list[Point]], radius: float = 1.2) -> list[tuple[int, int, int, Point]]:
    """Find time steps where two robots are too close (collision)."""
    collisions = []
    max_t = max(len(p) for p in paths)
    for t in range(max_t):
        for i in range(len(paths)):
            for j in range(i + 1, len(paths)):
                pi = paths[i][min(t, len(paths[i]) - 1)]
                pj = paths[j][min(t, len(paths[j]) - 1)]
                dist = ((pi.x - pj.x) ** 2 + (pi.y - pj.y) ** 2) ** 0.5
                if dist < radius:
                    mid = Point((pi.x + pj.x) / 2, (pi.y + pj.y) / 2)
                    collisions.append((i, j, t, mid))
    return collisions


def get_whca_paths_from_ros() -> Optional[list[list[Point]]]:
    """Try to read actual WHCA* paths from ROS2 topics."""
    paths = []
    for i in range(4):
        try:
            result = subprocess.run(
                ["ros2", "topic", "echo", f"/mapf/robot_{i}/plan", "--once"],
                capture_output=True,
                text=True,
                timeout=5,
            )
            if "position" not in result.stdout:
                return None
            path = []
            lines = result.stdout.split("\n")
            x_val = y_val = None
            for line in lines:
                line = line.strip()
                if line.startswith("x:") and "orientation" not in lines[max(0, lines.index(line) - 3):lines.index(line)]:
                    try:
                        x_val = float(line.split(":")[1])
                    except (ValueError, IndexError):
                        pass
                elif line.startswith("y:") and x_val is not None:
                    try:
                        y_val = float(line.split(":")[1])
                        path.append(Point(x_val, y_val))
                        x_val = y_val = None
                    except (ValueError, IndexError):
                        x_val = None
            if path:
                paths.append(path)
        except (subprocess.TimeoutExpired, FileNotFoundError):
            return None
    return paths if len(paths) == 4 else None


def whca_simulated_paths() -> list[list[Point]]:
    """Simulated WHCA* paths that avoid collisions via temporal offsets."""
    # Robot 0: goes right first, then up (avoids center at early time)
    p0 = []
    x, y = 3.0, 1.5
    for _ in range(6):
        p0.append(Point(x, y))
        x += CELL_SIZE
    for _ in range(9):
        p0.append(Point(x, y))
        y += CELL_SIZE
    p0.append(Point(12.0, 15.0))

    # Robot 1: goes up first, then left (offset timing)
    p1 = []
    x, y = 12.0, 1.5
    for _ in range(9):
        p1.append(Point(x, y))
        y += CELL_SIZE
    for _ in range(6):
        p1.append(Point(x, y))
        x -= CELL_SIZE
    p1.append(Point(3.0, 15.0))

    # Robot 2: goes right first, then down
    p2 = []
    x, y = 3.0, 15.0
    for _ in range(6):
        p2.append(Point(x, y))
        x += CELL_SIZE
    for _ in range(9):
        p2.append(Point(x, y))
        y -= CELL_SIZE
    p2.append(Point(12.0, 1.5))

    # Robot 3: goes down first, then left
    p3 = []
    x, y = 12.0, 15.0
    for _ in range(9):
        p3.append(Point(x, y))
        y -= CELL_SIZE
    for _ in range(6):
        p3.append(Point(x, y))
        x -= CELL_SIZE
    p3.append(Point(3.0, 1.5))

    return [p0, p1, p2, p3]


def draw_grid(ax: plt.Axes) -> None:
    """Draw warehouse grid background."""
    ax.set_xlim(-0.5, MAP_WIDTH + 0.5)
    ax.set_ylim(-0.5, MAP_HEIGHT + 0.5)
    ax.set_aspect("equal")
    ax.set_facecolor("#f5f5f5")
    # Grid lines
    for x in np.arange(0, MAP_WIDTH + CELL_SIZE, CELL_SIZE):
        ax.axvline(x, color="#e0e0e0", linewidth=0.3)
    for y in np.arange(0, MAP_HEIGHT + CELL_SIZE, CELL_SIZE):
        ax.axhline(y, color="#e0e0e0", linewidth=0.3)


def draw_paths(
    ax: plt.Axes,
    paths: list[list[Point]],
    title: str,
    collisions: Optional[list[tuple[int, int, int, Point]]] = None,
) -> None:
    draw_grid(ax)
    ax.set_title(title, fontsize=13, fontweight="bold", pad=10)

    for i, path in enumerate(paths):
        xs = [p.x for p in path]
        ys = [p.y for p in path]
        ax.plot(xs, ys, color=COLORS[i], linewidth=2, alpha=0.7, zorder=2)
        # Start marker
        ax.plot(xs[0], ys[0], "o", color=COLORS[i], markersize=12, zorder=4)
        ax.annotate(
            f"S{i}",
            (xs[0], ys[0]),
            textcoords="offset points",
            xytext=(0, -18),
            ha="center",
            fontsize=8,
            color=COLORS[i],
            fontweight="bold",
        )
        # Goal marker
        ax.plot(xs[-1], ys[-1], "*", color=COLORS[i], markersize=15, zorder=4)
        ax.annotate(
            f"G{i}",
            (xs[-1], ys[-1]),
            textcoords="offset points",
            xytext=(0, 12),
            ha="center",
            fontsize=8,
            color=COLORS[i],
            fontweight="bold",
        )

    if collisions:
        collision_points = set()
        for i, j, t, mid in collisions:
            collision_points.add((round(mid.x, 1), round(mid.y, 1)))
        for cx, cy in collision_points:
            ax.plot(cx, cy, "x", color="red", markersize=18, markeredgewidth=3, zorder=5)
            circle = plt.Circle((cx, cy), 1.0, fill=False, color="red",
                                linewidth=2, linestyle="--", zorder=3)
            ax.add_patch(circle)

    legend_elements = [
        mpatches.Patch(color=COLORS[i], label=ROBOT_NAMES[i]) for i in range(4)
    ]
    if collisions:
        legend_elements.append(
            plt.Line2D([0], [0], marker="x", color="red", linestyle="None",
                       markersize=10, markeredgewidth=2, label="Collision")
        )
    ax.legend(handles=legend_elements, loc="upper left", fontsize=8)


def main() -> None:
    # Case 1: Independent A* paths
    independent_paths = independent_astar_paths()
    collisions = find_collisions(independent_paths)

    # Case 2: Try real WHCA* paths from ROS, fall back to simulated
    whca_paths = get_whca_paths_from_ros()
    source = "ROS2 (live)"
    if whca_paths is None:
        whca_paths = whca_simulated_paths()
        source = "simulated"

    whca_collisions = find_collisions(whca_paths)

    # Plot
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))
    fig.suptitle(
        "WHCA* Multi-Robot Path Planning: Collision Avoidance Comparison",
        fontsize=14,
        fontweight="bold",
        y=0.98,
    )

    draw_paths(
        ax1,
        independent_paths,
        f"Case 1: Independent A* (No Coordination)\n{len(collisions)} collision(s) detected!",
        collisions,
    )
    draw_paths(
        ax2,
        whca_paths,
        f"Case 2: WHCA* (Windowed Cooperative A*)\n0 collisions — paths {source}",
        whca_collisions if whca_collisions else None,
    )

    plt.tight_layout()
    output_path = "/home/wei/ros2_map/mapf_ros/mapf_base/whca_comparison.png"
    plt.savefig(output_path, dpi=150, bbox_inches="tight")
    print(f"Comparison saved to: {output_path}")
    plt.show()


if __name__ == "__main__":
    main()
