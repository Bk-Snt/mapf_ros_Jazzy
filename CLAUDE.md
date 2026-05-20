# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is a ROS2 (Humble) workspace containing **mapf_ros** — a Multi-Agent Path Finding (MAPF) system that wraps MAPF algorithms as nav2-style plugins. It provides CBS (optimal), ECBS (suboptimal/fast), and Prioritized SIPP planners for multi-robot coordination on occupancy grid maps.

## Build

```bash
# From workspace root (parent of src/)
colcon build --symlink-install

# Build a single package
colcon build --symlink-install --packages-select mapf_ros
colcon build --symlink-install --packages-select mapf_base
colcon build --symlink-install --packages-select mapf_msgs
```

Source the workspace after building:
```bash
source install/setup.bash
```

## Launch

```bash
ros2 launch mapf_base mapf_example.launch.py map:=warehouse_low_reso_1.5.yaml
```

Select planner via the `mapf_planner` parameter (default in launch: `mapf_planner/ECBSROS`). Options:
- `mapf_planner/CBSROS` — optimal, exponential worst-case
- `mapf_planner/ECBSROS` — bounded suboptimal (configurable `suboptimality` factor)
- `mapf_planner/PrioritizedSIPPROS` — priority-based, incomplete (swap conflicts not handled)

## Architecture

Three ROS2 packages:

| Package | Role |
|---------|------|
| **mapf_msgs** | Custom messages: `Goal`, `SinglePlan`, `GlobalPlan` |
| **mapf_ros** | Planner plugins (shared libs loaded via `pluginlib`) |
| **mapf_base** | Central node (`mapf_base_node`) + example helpers (`goal_transformer`, `plan_executor`) |

### Plugin system

Each planner implements the `mapf::MAPFROS` interface (`mapf_ros/include/mapf_ros/mapf_ros.hpp`):
- `initialize(name, costmap_ros, node)`
- `makePlan(start, goal, plan, cost, time_tolerance)`

Plugins are declared in `*_planner_plugins.xml` files and loaded at runtime by `mapf_base_node` through `pluginlib`.

### Data flow

1. `goal_transformer` collects per-agent goals → publishes `mapf_msgs/Goal`
2. `mapf_base_node` receives goal, runs selected planner on the costmap → publishes `mapf_msgs/GlobalPlan`
3. `plan_executor` dispatches plan waypoints to each agent's `move_base/goal` by time step

### Key design constraint

Use **low-resolution maps** for MAPF planning (CBS/ECBS are space-time searches, high-res maps cause exponential blowup). The minimum grid step must exceed robot diameter to guarantee collision-free execution.

## Dependencies

- ROS2 Humble
- nav2 stack (`nav2_costmap_2d`, `nav2_util`, `nav2_map_server`, `nav2_lifecycle_manager`)
- Boost (thread)
- `pluginlib`, `tf2_ros`

## Configuration

- `mapf_base/params/mapf_params.yaml` — agent count, frame IDs, planner timeout, goal tolerance, ECBS suboptimality
- `mapf_base/params/costmap_params.yaml` — nav2 costmap configuration
