import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.event_handlers import OnProcessStart
from launch_ros.actions import Node

def generate_launch_description():

    # 1. Include State Publisher with simulation tracking turned off
    robot_state_pub = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('robot_2_bringup'), 'launch', 'robot_state_publisher.launch.py'
        )]), launch_arguments={'use_sim_time': 'false', 'sim_mode': 'false'}.items()
    )

    # 2. Configure Controller Manager node explicitly for hardware environment
    controller_params = os.path.join(
        get_package_share_directory('robot_2_bringup'), 'config', 'my_controllers.yaml'
    )
    
    controller_manager = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[controller_params],
        remappings=[
            ('/controller_manager/robot_description', '/robot_description')
        ],
        output='screen'
    )

    # 3. Define Controller Spawners
    joint_broad_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_broad'],
        output='screen'
    )

    diff_cont_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['diff_cont'],
        output='screen'
    )

    # 4. Chain processes systematically to avoid initialization race conditions
    delay_controller_manager = TimerAction(
        period=3.0,
        actions=[controller_manager]
    )

    delay_joint_broad_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_process=controller_manager,
            on_start=[joint_broad_spawner]
        )
    )

    delay_diff_cont_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_process=controller_manager,
            on_start=[diff_cont_spawner]
        )
    )

    return LaunchDescription([
        robot_state_pub,
        delay_controller_manager,
        delay_joint_broad_spawner,
        delay_diff_cont_spawner
    ])
