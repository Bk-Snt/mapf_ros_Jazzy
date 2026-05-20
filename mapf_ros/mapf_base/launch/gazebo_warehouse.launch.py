import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    GroupAction,
    IncludeLaunchDescription,
    TimerAction,
)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    mapf_base_share = get_package_share_directory("mapf_base")

    world_file = os.path.join(mapf_base_share, "worlds", "warehouse_ign.sdf")
    map_file = os.path.join(mapf_base_share, "maps", "warehouse.yaml")
    mapf_params = os.path.join(
        mapf_base_share, "params", "whca_warehouse_params.yaml"
    )
    costmap_params = os.path.join(
        mapf_base_share, "params", "whca_costmap_params.yaml"
    )

    # Arguments
    use_sim_time_arg = DeclareLaunchArgument(
        "use_sim_time", default_value="true"
    )
    window_size_arg = DeclareLaunchArgument(
        "window_size", default_value="16"
    )

    use_sim_time = LaunchConfiguration("use_sim_time")

    robots = ["robot_0", "robot_1", "robot_2", "robot_3"]

    # 1. Launch Ignition Gazebo with warehouse world
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("ros_gz_sim"),
                "launch",
                "gz_sim.launch.py",
            )
        ),
        launch_arguments={
            "gz_args": f"-r {world_file}",
            "on_exit_shutdown": "true",
        }.items(),
    )

    # 2. Bridge: Ignition topics <-> ROS2 topics
    # Clock + per-robot odom and cmd_vel
    bridge_args = [
        "/clock@rosgraph_msgs/msg/Clock[ignition.msgs.Clock",
    ]
    for robot in robots:
        bridge_args.extend([
            f"/{robot}/cmd_vel@geometry_msgs/msg/Twist]ignition.msgs.Twist",
            f"/{robot}/odom@nav_msgs/msg/Odometry[ignition.msgs.Odometry",
        ])

    ros_gz_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=bridge_args,
        output="screen",
        parameters=[{"use_sim_time": use_sim_time}],
    )

    # 3. TF bridge: Ignition publishes odom->base_footprint via /tf
    # We also need map->odom static transforms
    # Ignition DiffDrive publishes TF on /tf internally via the bridge
    tf_bridge_args = [
        "/tf@tf2_msgs/msg/TFMessage[ignition.msgs.Pose_V",
    ]
    tf_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        name="tf_bridge",
        arguments=tf_bridge_args,
        output="screen",
        parameters=[{"use_sim_time": use_sim_time}],
    )

    # 4. Static TF: map -> robot_N/odom
    static_tf_nodes = []
    for robot in robots:
        static_tf_nodes.append(
            Node(
                package="tf2_ros",
                executable="static_transform_publisher",
                name="tf_map_" + robot,
                output="screen",
                arguments=[
                    "--x", "0", "--y", "0", "--z", "0",
                    "--roll", "0", "--pitch", "0", "--yaw", "0",
                    "--frame-id", "map",
                    "--child-frame-id", robot + "/odom",
                ],
            )
        )

    # 5. MAPF planning stack
    lifecycle_nodes = ["map_server", "mapf_base_node"]
    mapf_nodes = GroupAction(
        [
            Node(
                namespace="mapf",
                package="nav2_map_server",
                executable="map_server",
                name="map_server",
                output="screen",
                respawn=True,
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
                respawn=True,
                parameters=[
                    costmap_params,
                    mapf_params,
                    {"mapf_planner": "mapf_planner/WHCAROS"},
                    {
                        "whca.window_size": LaunchConfiguration("window_size")
                    },
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
                    {"autostart": True},
                    {"node_names": lifecycle_nodes},
                    {"bond_timeout": 30.0},
                    {"attempt_respawn_reconnection": True},
                ],
            ),
        ]
    )

    # 6. Goal transformer and plan executor (delayed)
    helpers = TimerAction(
        period=15.0,
        actions=[
            GroupAction(
                [
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
                    Node(
                        namespace="mapf",
                        package="mapf_base",
                        executable="plan_executor",
                        name="plan_executor",
                        output="screen",
                        parameters=[
                            mapf_params,
                            {"use_sim_time": use_sim_time},
                        ],
                    ),
                ]
            )
        ],
    )

    return LaunchDescription(
        [
            use_sim_time_arg,
            window_size_arg,
            gz_sim,
            ros_gz_bridge,
            tf_bridge,
            *static_tf_nodes,
            TimerAction(period=10.0, actions=[mapf_nodes]),
            helpers,
        ]
    )
