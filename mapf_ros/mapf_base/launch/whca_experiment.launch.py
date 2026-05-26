"""
Launch file for WHCA* Silver 2005 replication experiments.

Starts:
  - whca_experiment_node.py  (generates maps, runs WHCA*, publishes to RViz)
  - RViz2                    (with the existing whca_experiment.rviz config)

Usage:
  ros2 launch mapf_base whca_experiment.launch.py
  ros2 launch mapf_base whca_experiment.launch.py n_trials:=3
  ros2 launch mapf_base whca_experiment.launch.py agent_counts:=10,20,30 window_sizes:=16


"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():

    # ── Launch arguments (defaults match Silver 2005) ─────────
    args = [
        DeclareLaunchArgument(
            "window_sizes", default_value="8,16,32",
            description="Comma-separated WHCA* window sizes to test",
        ),
        DeclareLaunchArgument(
            "agent_counts", default_value="10,20,30,40,50,60,70,80,90,100",
            description="Comma-separated agent counts to test",
        ),
        DeclareLaunchArgument(
            "n_trials", default_value="10",
            description="Number of random maps (trials) per configuration",
        ),
        DeclareLaunchArgument(
            "max_turns", default_value="100",
            description="Success criterion: agent must arrive within this many turns",
        ),
        DeclareLaunchArgument(
            "animate_delay", default_value="4.0",
            description="Seconds to display each result in RViz before moving on",
        ),
        DeclareLaunchArgument(
            "cell_size", default_value="0.5",
            description="Metres per grid cell (affects RViz scale only)",
        ),
    ]

    rviz_config = PathJoinSubstitution([
        FindPackageShare("mapf_base"), "params", "whca_experiment.rviz"
    ])

    # ── Experiment node ──────────────────────────────────────────────────────
    experiment_node = Node(
        namespace="mapf",
        package="mapf_base",
        executable="whca_experiment_node.py",
        name="whca_experiment_node",
        output="screen",
        parameters=[{
            "window_sizes":  LaunchConfiguration("window_sizes"),
            "agent_counts":  LaunchConfiguration("agent_counts"),
            "n_trials":      LaunchConfiguration("n_trials"),
            "max_turns":     LaunchConfiguration("max_turns"),
            "animate_delay": LaunchConfiguration("animate_delay"),
            "cell_size":     LaunchConfiguration("cell_size"),
            "global_frame":  "map",
        }],
    )

    # ── RViz ─────────────────────────────────────────────────────────────────
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        arguments=["-d", rviz_config],
        output="screen",
    )

    return LaunchDescription(args + [experiment_node, rviz_node])
