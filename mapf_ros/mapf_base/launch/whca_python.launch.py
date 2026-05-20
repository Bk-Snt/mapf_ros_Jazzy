"""
Launch WHCA* using the Python implementation.

This replaces the C++ mapf_base_node + WHCA plugin with a single Python node.
Everything else (map_server, plan_animator, goal_transformer) remains the same.

Usage:
  ros2 launch mapf_base whca_python.launch.py
  ros2 launch mapf_base whca_python.launch.py window_size:=64 map:=warehouse_simple.yaml
"""
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction, TimerAction
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    default_map = PathJoinSubstitution(
        [FindPackageShare("mapf_base"), "maps", "warehouse.yaml"]
    )
    map_file_arg = DeclareLaunchArgument(
        name="map",
        default_value=default_map,
        description="Full path to the map YAML file",
    )
    use_sim_time_arg = DeclareLaunchArgument(
        name="use_sim_time",
        default_value="false",
    )
    window_size_arg = DeclareLaunchArgument(
        name="window_size",
        default_value="48",
        description="WHCA* planning window size (time steps)",
    )
    replan_interval_arg = DeclareLaunchArgument(
        name="replan_interval",
        default_value="2.0",
        description="Seconds between replan checks (0=disabled)",
    )

    map_file = LaunchConfiguration("map")
    use_sim_time = LaunchConfiguration("use_sim_time")

    mapf_params = PathJoinSubstitution(
        [FindPackageShare("mapf_base"), "params", "whca_warehouse_params.yaml"]
    )

    # 8 robots at 2 charging stations (4 per station)
    # Station A: bottom-left, Station B: bottom-right
    robots = [
        {"name": "robot_0", "x": 1.5, "y": 1.5},
        {"name": "robot_1", "x": 1.5, "y": 3.0},
        {"name": "robot_2", "x": 3.0, "y": 1.5},
        {"name": "robot_3", "x": 3.0, "y": 3.0},
        {"name": "robot_4", "x": 40.5, "y": 1.5},
        {"name": "robot_5", "x": 40.5, "y": 3.0},
        {"name": "robot_6", "x": 39.0, "y": 1.5},
        {"name": "robot_7", "x": 39.0, "y": 3.0},
    ]

    # Plan animator: publishes TF + markers for visualization
    animator_params = {}
    for i, robot in enumerate(robots):
        animator_params[f"init_pos.agent_{i}.x"] = robot["x"]
        animator_params[f"init_pos.agent_{i}.y"] = robot["y"]

    plan_animator_node = Node(
        namespace="mapf",
        package="mapf_base",
        executable="plan_animator",
        name="plan_animator",
        output="screen",
        parameters=[
            mapf_params,
            {"use_sim_time": use_sim_time},
            {"step_duration": 1.5},
            {"agent_num": 8},
            animator_params,
        ],
    )

    # Python WHCA* node (replaces mapf_base_node + plugin)
    whca_python_node = Node(
        namespace="mapf",
        package="mapf_base",
        executable="whca_node.py",
        name="whca_node",
        output="screen",
        parameters=[
            mapf_params,
            {"use_sim_time": use_sim_time},
            {"window_size": LaunchConfiguration("window_size")},
            {"replan_interval": LaunchConfiguration("replan_interval")},
        ],
    )

    # Map server (needs lifecycle management)
    map_server_node = Node(
        namespace="mapf",
        package="nav2_map_server",
        executable="map_server",
        name="map_server",
        output="screen",
        parameters=[
            {"yaml_filename": map_file},
            {"use_sim_time": use_sim_time},
        ],
    )

    lifecycle_manager_node = Node(
        namespace="mapf",
        package="nav2_lifecycle_manager",
        executable="lifecycle_manager",
        name="lifecycle_manager_mapf",
        output="screen",
        parameters=[
            {"use_sim_time": use_sim_time},
            {"autostart": True},
            {"node_names": ["map_server"]},
            {"bond_timeout": 30.0},
        ],
    )

    # Goal transformer (delayed start to let map load)
    goal_transformer_node = TimerAction(
        period=3.0,
        actions=[
            Node(
                namespace="mapf",
                package="mapf_base",
                executable="goal_transformer",
                name="goal_transformer",
                output="screen",
                parameters=[
                    mapf_params,
                    {"use_sim_time": use_sim_time},
                ],
            ),
        ],
    )

    return LaunchDescription(
        [
            map_file_arg,
            use_sim_time_arg,
            window_size_arg,
            replan_interval_arg,
            plan_animator_node,
            GroupAction([map_server_node, lifecycle_manager_node]),
            whca_python_node,
            goal_transformer_node,
        ]
    )
