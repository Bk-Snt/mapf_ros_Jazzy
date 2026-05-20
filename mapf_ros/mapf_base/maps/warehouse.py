#!/usr/bin/env python3
"""Generate warehouse occupancy grid maps of varying complexity."""

from typing import Optional


def generate_warehouse_pgm(
    filename: str,
    width: int,
    height: int,
    layout: str = "complex",
) -> None:
    grid = [[255] * width for _ in range(height)]

    # Walls around perimeter
    for x in range(width):
        grid[0][x] = 0
        grid[height - 1][x] = 0
    for y in range(height):
        grid[y][0] = 0
        grid[y][width - 1] = 0

    if layout == "simple":
        # Original: 4 pairs of shelf columns
        shelf_cols = [4, 5, 9, 10, 14, 15, 19, 20]
        for col in shelf_cols:
            for row in range(3, 8):
                grid[row][col] = 0
            for row in range(12, 17):
                grid[row][col] = 0

    elif layout == "complex":
        # Dense warehouse: more shelves, narrower aisles, loading docks
        # 6 shelf blocks (3 columns × 2 vertical sections)
        shelf_pairs = [(3, 4), (7, 8), (11, 12), (15, 16), (19, 20), (23, 24)]
        for c1, c2 in shelf_pairs:
            if c1 >= width - 1 or c2 >= width - 1:
                continue
            # Lower shelf block
            for row in range(3, 9):
                grid[row][c1] = 0
                grid[row][c2] = 0
            # Upper shelf block
            for row in range(11, 17):
                grid[row][c1] = 0
                grid[row][c2] = 0
            # Middle crossbar connecting the pair
            for row in range(6, 7):
                grid[row][c1] = 0
                grid[row][c2] = 0
            for row in range(13, 14):
                grid[row][c1] = 0
                grid[row][c2] = 0

        # Loading dock walls on right side
        for row in range(2, 5):
            grid[row][width - 2] = 0
        for row in range(height - 5, height - 2):
            grid[row][width - 2] = 0

        # Central pillar obstacles
        grid[10][13] = 0
        grid[10][14] = 0
        grid[9][13] = 0
        grid[9][14] = 0

    elif layout == "maze":
        # Maze-like: many narrow corridors forcing complex routing
        # Horizontal walls with gaps
        for x in range(2, width - 2):
            if x not in (5, 10, 15, 20, 25):
                grid[4][x] = 0
        for x in range(2, width - 2):
            if x not in (3, 8, 13, 18, 23, 27):
                grid[8][x] = 0
        for x in range(2, width - 2):
            if x not in (5, 10, 15, 20, 25):
                grid[12][x] = 0
        for x in range(2, width - 2):
            if x not in (3, 8, 13, 18, 23, 27):
                grid[16][x] = 0

        # Vertical walls with gaps
        for y in range(2, height - 2):
            if y not in (3, 6, 10, 14, 17):
                grid[y][6] = 0
        for y in range(2, height - 2):
            if y not in (2, 5, 9, 13, 17):
                grid[y][14] = 0
        for y in range(2, height - 2):
            if y not in (3, 7, 11, 15, 18):
                grid[y][22] = 0

    # Write PGM (P5 binary format)
    with open(filename, "wb") as f:
        header = f"P5\n{width} {height}\n255\n"
        f.write(header.encode())
        for row in reversed(grid):
            f.write(bytes(row))


if __name__ == "__main__":
    import sys

    layout = sys.argv[1] if len(sys.argv) > 1 else "complex"
    width = 30
    height = 20

    filename = f"/home/wei/ros2_map/mapf_ros/mapf_base/maps/warehouse_{layout}.pgm"
    generate_warehouse_pgm(filename, width=width, height=height, layout=layout)
    print(f"Generated {filename} ({width}x{height}, layout='{layout}', resolution 1.5m/cell)")
    print(f"World size: {width * 1.5}m x {height * 1.5}m")

    # Also write as warehouse.pgm (default) if explicitly requested
    if len(sys.argv) > 1:
        default_path = "/home/wei/ros2_map/mapf_ros/mapf_base/maps/warehouse.pgm"
        generate_warehouse_pgm(default_path, width=width, height=height, layout=layout)
        print(f"Also wrote {default_path}")
