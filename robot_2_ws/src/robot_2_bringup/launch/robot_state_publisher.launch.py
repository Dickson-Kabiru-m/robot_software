import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node
import xacro

def generate_launch_description():
  # Check arguments passed from parent launch scripts
  use_sim_time = LaunchConfiguration('use_sim_time')
  sim_mode = LaunchConfiguration('sim_mode')

  # Process the URDF file inside the description directory
  pkg_path = os.path.join(get_package_share_directory('robot_2_description'))
  xacro_file = os.path.join(pkg_path, 'urdf', 'robot_2.urdf.xacro')
  
  # Pass the launch configuration argument directly into xacro processor mappings
  robot_description_config = xacro.process_file(
      xacro_file, 
      mappings={'sim_mode': sim_mode}
  ).toxml()

  # Create a robot_state_publisher node tracking the mandatory layout guidelines
  params = {'robot_description': robot_description_config, 'use_sim_time': use_sim_time}
  node_robot_state_publisher = Node(
    package='robot_state_publisher',
    executable='robot_state_publisher',
    namespace='robot_2',
    name='robot_state_publisher',
    output='screen',
    parameters=[params]
  )

  return LaunchDescription([
    DeclareLaunchArgument(
      'use_sim_time',
      default_value='false',
      description='Use sim time if true'),
    DeclareLaunchArgument(
      'sim_mode',
      default_value='false',
      description='Use simulation parameters if true'),
    node_robot_state_publisher
  ])
