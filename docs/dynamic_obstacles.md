# Dynamic Obstacles

The WHCA* planner supports dynamic obstacles via the `/mapf/dynamic_obstacles` topic (`nav_msgs/OccupancyGrid`). When obstacles change and `replan_interval > 0`, the planner automatically replans from current robot positions.

Dynamic obstacles appear as red cubes in RViz.

## Publishing dynamic obstacles

In a separate terminal, publish an OccupancyGrid to block cells. The grid must match the map dimensions (30x20) and resolution (1.5m/cell):

```bash
source install/setup.bash

# Block a single cell at grid position (15, 10) — center of the warehouse
ros2 topic pub /mapf/dynamic_obstacles nav_msgs/msg/OccupancyGrid "{
  header: {frame_id: 'map'},
  info: {resolution: 1.5, width: 30, height: 20, origin: {position: {x: 0, y: 0}}},
  data: [$(python3 -c "
import sys
grid = [0]*600
# Block cells: set to 100 (occupied). Index = y * width + x
grid[10*30 + 15] = 100  # cell (15, 10)
grid[10*30 + 16] = 100  # cell (16, 10)
grid[11*30 + 15] = 100  # cell (15, 11)
print(','.join(map(str, grid)))
")]
}" --once
```

## Python script

For more control, use a Python script:

```python
#!/usr/bin/env python3
"""Publish dynamic obstacles to trigger WHCA* replanning."""
import rclpy
from rclpy.node import Node
from nav_msgs.msg import OccupancyGrid
import numpy as np

rclpy.init()
node = Node('obstacle_publisher')
pub = node.create_publisher(OccupancyGrid, '/mapf/dynamic_obstacles', 10)

msg = OccupancyGrid()
msg.header.frame_id = 'map'
msg.info.resolution = 1.5
msg.info.width = 30
msg.info.height = 20

# Create obstacle grid (0=free, 100=occupied)
grid = np.zeros(30 * 20, dtype=np.int8)
# Block a corridor in the center
for x in range(12, 18):
    grid[10 * 30 + x] = 100

msg.data = grid.tolist()

import time
time.sleep(1)  # Wait for subscriber discovery
pub.publish(msg)
node.get_logger().info('Published dynamic obstacles')
node.destroy_node()
rclpy.shutdown()
```

## Behavior

- The planner detects changes and replans within `replan_interval` seconds (default: 2.0)
- Robots already past the blocked area continue on their current path
- Robots approaching the blocked area will route around it
- Publishing an empty grid (all zeros) clears all dynamic obstacles

## Moving obstacles

To simulate a moving obstacle, publish updated grids on a timer. Each publish replaces the previous dynamic obstacle layer entirely — the planner merges it with the static map on each replan cycle.
