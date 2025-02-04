cmake_minimum_required(VERSION 2.8.3)
project(ohm_robotarm)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED roscpp std_msgs sensor_msgs geometry_msgs message_generation ohm_can_interface 
             hardware_interface controller_manager control_msgs trajectory_msgs actionlib moveit_ros_planning_interface 
             trac_ik_lib trac_ik_kinematics_plugin joy kdl_parser urdf ohm_actors_msgs)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################
## SET C++ 11 ##
################

set(CMAKE_CXX_FLAGS "-std=c++11")

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
add_message_files(
	FILES
	DriveState.msg
)

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
generate_messages(
   DEPENDENCIES
   std_msgs  # Or other packages containing msgs
 )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
   INCLUDE_DIRS 
   include
#  LIBRARIES canopen_lib
   CATKIN_DEPENDS 
    hardware_interface
    controller_manager
    roscpp
    control_msgs
    trajectory_msgs
    ohm_can_interface 
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

#link_directories(~/workspace/ros/devel/lib) # todo: find solution to link for /home/johannes/workspace/catkin_ws/devel/lib/ohm_can_interface
#link_directories(/home/timo/Dokumente/catkin_ws/devel/lib)
## Build moveit_interface
add_executable(moveit_control_interface           src/moveit_control_interface.cpp
                                                  src/moveit_interface_control_loop.cpp)
								          
add_dependencies(moveit_control_interface         ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(moveit_control_interface    ${catkin_LIBRARIES})

## Build sim_drives
add_executable(sim_drives                 src/sim_drives.cpp
                                          src/drive.cpp)
								          
add_dependencies(sim_drives               ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(sim_drives          ${catkin_LIBRARIES})

## Build elmo_drives
add_executable(elmo_drives                src/elmo_drives.cpp
                                          src/drive.cpp)
								          
add_dependencies(elmo_drives              ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(elmo_drives         ${catkin_LIBRARIES})

## Build dynamixel_drives
add_executable(dynamixel_drives            src/dynamixel_drives.cpp
                                           src/serial_rs232.cpp
                                           src/drive.cpp)
								          
add_dependencies(dynamixel_drives          ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(dynamixel_drives     ${catkin_LIBRARIES})								          

## Build joint state publisher
add_executable(joint_state_publisher       src/joint_state_publisher.cpp
                                           src/drive.cpp)
								          
add_dependencies(joint_state_publisher      ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(joint_state_publisher ${catkin_LIBRARIES})

## Build gripper_interface
add_executable(gripper_interface           src/gripper_interface.cpp
                                           src/drive.cpp)
								          
add_dependencies(gripper_interface         ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(gripper_interface    ${catkin_LIBRARIES})	
								          
#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
 #install(TARGETS ohm_can_interface ohm_can_interface_node
 #  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
 #  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
 #  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 #)

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_ohm_can_interface.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
