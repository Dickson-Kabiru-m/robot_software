import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration

from launch_ros.actions import Node

import xacro


def launch_robot_state_publisher(context, *args, **kwargs):

    use_sim_time = LaunchConfiguration(
        'use_sim_time'
    ).perform(context)

    sim_mode = LaunchConfiguration(
        'sim_mode'
    ).perform(context)

    description_package = get_package_share_directory(
        'robot_2_description'
    )

    xacro_file = os.path.join(
        description_package,
        'urdf',
        'robot_2.urdf.xacro'
    )

    robot_description_config = xacro.process_file(
        xacro_file,
        mappings={
            'sim_mode': sim_mode
        }
    ).toxml()

    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',

        namespace='robot_2',
        name='robot_state_publisher',

        output='screen',

        parameters=[
            {
                'robot_description': robot_description_config,
                'use_sim_time': use_sim_time,
            }
        ],
    )

    return [robot_state_publisher]


def generate_launch_description():

    return LaunchDescription([

        DeclareLaunchArgument(
            'use_sim_time',
            default_value='false',
            description='Use simulation time'
        ),

        DeclareLaunchArgument(
            'sim_mode',
            default_value='false',
            description='Use simulation ros2_control'
        ),

        OpaqueFunction(
            function=launch_robot_state_publisher
        ),
    ])