import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():

    # ==========================================================
    # Launch arguments
    # ==========================================================

    sim_mode = LaunchConfiguration("sim_mode")

    declare_sim_mode = DeclareLaunchArgument(
        "sim_mode",
        default_value="false",
        description="Run the robot in simulation mode"
    )

    # ==========================================================
    # Package paths
    # ==========================================================

    description_package = FindPackageShare(
        "robot_2_description"
    )

    bringup_package = FindPackageShare(
        "robot_2_bringup"
    )

    xacro_file = PathJoinSubstitution([
        description_package,
        "urdf",
        "robot_2.urdf.xacro"
    ])

    controllers_file = PathJoinSubstitution([
        bringup_package,
        "config",
        "my_controllers.yaml"
    ])

    # ==========================================================
    # Robot description
    # ==========================================================

    robot_description = Command([
        "xacro ",
        xacro_file,
        " sim_mode:=",
        sim_mode
    ])

    # ==========================================================
    # Robot State Publisher
    # ==========================================================

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        namespace="robot_2",
        name="robot_state_publisher",

        parameters=[
            {
                "robot_description": ParameterValue(
                    robot_description,
                    value_type=str
                ),

                "use_sim_time": ParameterValue(
                    sim_mode,
                    value_type=bool
                ),
		"frame_prefix":"robot_2/",
            }
        ],

        output="screen"
    )

    # ==========================================================
    # ROS 2 Control Controller Manager
    # ==========================================================

    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        namespace="robot_2",
        name="controller_manager",

        parameters=[
            controllers_file,
        ],

        output="screen"
    )

    # ==========================================================
    # Joint State Broadcaster
    # ==========================================================

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        namespace="robot_2",

        arguments=[
            "joint_broad"
        ],

        output="screen"
    )

    # ==========================================================
    # Differential Drive Controller
    # ==========================================================

    diff_drive_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        namespace="robot_2",

        arguments=[
            "diff_cont"
        ],

        output="screen"
    )

    # ==========================================================
    # Launch description
    # ==========================================================

    return LaunchDescription([
        declare_sim_mode,
        robot_state_publisher,
        ros2_control_node,
        joint_state_broadcaster_spawner,
        diff_drive_controller_spawner,
    ])

