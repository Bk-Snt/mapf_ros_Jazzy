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
        description="Use simulation (Gazebo) clock if true",
    )
    autostart_arg = DeclareLaunchArgument(
        name="autostart",
        default_value="true",
        description="Automatically startup the nav2 stack",
    )
    window_size_arg = DeclareLaunchArgument(
        name="window_size",
        default_value="48",
        description="WHCA* planning window size (time steps)",
    )
    timeout_arg = DeclareLaunchArgument(
        name="timeout",
        default_value="60.0",
        description="Planner timeout in seconds",
    )

    map_file = LaunchConfiguration("map")
    use_sim_time = LaunchConfiguration("use_sim_time")
    autostart = LaunchConfiguration("autostart")
    timeout = LaunchConfiguration("timeout")

    lifecycle_nodes = ["map_server", "mapf_base_node"]

    mapf_params = PathJoinSubstitution(
        [
            FindPackageShare("mapf_base"),
            "params",
            "whca_warehouse_params.yaml",
        ]
    )
    costmap_params = PathJoinSubstitution(
        [
            FindPackageShare("mapf_base"),
            "params",
            "whca_costmap_params.yaml",
        ]
    )

    # 8 robots spread across the warehouse (30x20 grid at 1.5m/cell = 45m x 30m)
    # Placed in free aisles (col 1,5,9,13,17,21,25,27 are free corridors)
    robots = [
        {"name": "robot_0", "x": 1.5, "y": 1.5},     # cell (1,1) bottom-left
        {"name": "robot_1", "x": 7.5, "y": 1.5},     # cell (5,1)
        {"name": "robot_2", "x": 13.5, "y": 1.5},    # cell (9,1)
        {"name": "robot_3", "x": 28.5, "y": 1.5},    # cell (19,1) bottom-right area
        {"name": "robot_4", "x": 1.5, "y": 27.0},    # cell (1,18) top-left
        {"name": "robot_5", "x": 7.5, "y": 27.0},    # cell (5,18)
        {"name": "robot_6", "x": 13.5, "y": 27.0},   # cell (9,18)
        {"name": "robot_7", "x": 28.5, "y": 27.0},   # cell (19,18) top-right area
    ]

    # Plan animator: publishes dynamic TF and markers for all robots
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

    return LaunchDescription(
        [
            map_file_arg,
            use_sim_time_arg,
            autostart_arg,
            window_size_arg,
            timeout_arg,
            plan_animator_node,
            GroupAction(
                [
                    Node(
                        namespace="mapf",
                        package="nav2_map_server",
                        executable="map_server",
                        name="map_server",
                        output="screen",
                        parameters=[
                            mapf_params,
                            {"yaml_filename": map_file},
                            {"use_sim_time": use_sim_time},
                        ],
                    ),
                    Node(
                        namespace="mapf",
                        package="mapf_base",
                        executable="mapf_base_node",
                        name="mapf_base_node",
                        output="screen",
                        parameters=[
                            costmap_params,
                            mapf_params,
                            {"mapf_planner": "mapf_planner/WHCAROS"},
                            {"whca.window_size": LaunchConfiguration("window_size")},
                            {"planner_time_tolerance": timeout},
                            {"use_sim_time": use_sim_time},
                        ],
                    ),
                    Node(
                        namespace="mapf",
                        package="nav2_lifecycle_manager",
                        executable="lifecycle_manager",
                        name="lifecycle_manager_mapf",
                        output="screen",
                        parameters=[
                            {"use_sim_time": use_sim_time},
                            {"autostart": autostart},
                            {"node_names": lifecycle_nodes},
                            {"bond_timeout": 30.0},
                        ],
                    ),
                ]
            ),
            TimerAction(
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
            ),
        ]
    )
