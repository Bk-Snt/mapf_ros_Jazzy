# WHCA* Silver 2005 Replication Experiments

Replicates Figures 3–6 from Silver (2005) "Cooperative Pathfinding" using
the WHCA* Python implementation on ROS 2 Jazzy.

## Prerequisites

- ROS 2 Jazzy installed
- Workspace built (see Jazzy setup below)

## First-time Jazzy setup

```bash
git clone https://github.com/Bk-Snt/mapf_ros_Jazzy.git ~/ros2_map
cd ~/ros2_map

# Hide old duplicate packages from colcon
touch mapf_base/COLCON_IGNORE
touch mapf_msgs/COLCON_IGNORE

# Apply Jazzy patches
cd mapf_ros/mapf_ros
find . -name "*.cpp" -exec sed -i \
  -e 's|tf2/utils\.h|tf2/utils.hpp|g' \
  -e 's|tf2_ros/transform_listener\.h|tf2_ros/transform_listener.hpp|g' \
  -e 's|mapf_msgs/msg/goal\.h|mapf_msgs/msg/goal.hpp|g' \
  -e 's|mapf_msgs/msg/single_plan\.h|mapf_msgs/msg/single_plan.hpp|g' {} +

cd ~/ros2_map/mapf_ros/mapf_base
sed -i 's/boost::shared_ptr<mapf::MAPFROS>/std::shared_ptr<mapf::MAPFROS>/g' \
  include/mapf_base/mapf_base.hpp
sed -i 's/createUniqueInstance/createSharedInstance/g' src/mapf_base.cpp
# Then open src/mapf_base.cpp and change the Costmap2DROS constructor
# (line ~42) to add get_parameter("use_sim_time").as_bool() as 4th argument

# Build
cd ~/ros2_map
source /opt/ros/jazzy/setup.bash
rosdep install --from-paths mapf_ros --ignore-src -r -y --rosdistro jazzy
colcon build --symlink-install
source install/setup.bash
```

## Run the Silver 2005 replication experiment (full)

```bash
cd ~/ros2_map
source /opt/ros/jazzy/setup.bash && source install/setup.bash
ros2 launch mapf_base whca_experiment.launch.py
```

**Runtime:** ~20–40 minutes (300 configurations: W=8/16/32 × 10 agent counts × 10 trials).

**What you see:** RViz opens showing a 32×32 random maze. For the first trial
of each configuration the planned paths animate — colored line strips per agent,
robot spheres moving along paths, goal rings. Terminal prints progress and
metrics live.

**Results saved to:** `~/ros2_map/whca_results.csv`

## Run a quick test (3 minutes)

```bash
ros2 launch mapf_base whca_experiment.launch.py \
  agent_counts:=10,30,50 window_sizes:=16 n_trials:=3
```

## Launch arguments

| Argument | Default | Description |
|----------|---------|-------------|
| `window_sizes` | `8,16,32` | Comma-separated WHCA* window sizes |
| `agent_counts` | `10,20,...,100` | Comma-separated agent counts |
| `n_trials` | `10` | Random maps per configuration |
| `max_turns` | `100` | Success criterion (turns) |
| `animate_delay` | `8.0` | Seconds to display each result |

## Run the supervisor's warehouse demo (8 agents)

```bash
ros2 launch mapf_base whca_python.launch.py
# Terminal 2:
python3 mapf_ros/mapf_base/scripts/whca_demo.py
```

## Experiment setup (matches Silver 2005)

- **Grid:** 32×32, 4-connected
- **Obstacles:** 20% random, disconnected regions filled (fully connected map)
- **Agents:** 10–100 in steps of 10
- **Window sizes:** W = 8, 16, 32
- **Trials:** 10 random maps per configuration
- **Success criterion:** agent reaches goal within 100 turns
- **Heuristic:** Manhattan distance (supervisor's implementation; Silver uses RRA*)

## Metrics collected

| Metric | Description |
|--------|-------------|
| `success_rate` | % agents reaching goal within 100 turns |
| `avg_path_len` | Average turns to reach goal (100 for failures) |
| `avg_cycles` | Average times an agent revisits a cell |
| `init_ms` | Time for first window planning call (ms) |

## Note on timing

Silver (2005) ran on a 1.2 GHz Pentium 4. Modern hardware is ~100× faster
so absolute ms values will not match. Compare trends across window sizes only.
