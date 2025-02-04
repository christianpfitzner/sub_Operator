cmake_minimum_required(VERSION 2.8.3)
project(ohm_dynamixel)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  ohm_actors_msgs
)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 --std=c++11")

## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## Please declare messages etc in the _msgs package in this meta package

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
#  INCLUDE_DIRS include
#  LIBRARIES ohm_dynamixel
  CATKIN_DEPENDS roscpp std_msgs ohm_actors_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp library
# add_library(ohm_dynamixel
#   src/${PROJECT_NAME}/ohm_dynamixel.cpp
# )

## Declare a cpp executable
add_executable(dynamixel-bus
               src/main.cpp
               src/Dynamixel.cpp
               src/DynamixelBus.cpp
               src/Mx106.cpp
               src/Mx28.cpp
               src/Ax12.cpp
               src/dynamixel.c
               src/dxl_hal.c
               )

add_executable(set-position-limits
               src/set-position-limits.cpp
               src/Dynamixel.cpp
               src/DynamixelBus.cpp
               src/Mx106.cpp
               src/Mx28.cpp
               src/Ax12.cpp
               src/dynamixel.c
               src/dxl_hal.c
               )

add_executable(dynamixel-server
               src/dynamixel-server.cpp
               src/Dynamixel.cpp
               src/DynamixelBus.cpp
               src/Mx106.cpp
               src/Mx28.cpp
               src/Ax12.cpp
               src/dynamixel.c
               src/dxl_hal.c
               )


add_executable(test_ass src/test.cpp)

add_dependencies(dynamixel-bus ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(set-position-limits ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(dynamixel-server ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(dynamixel-bus
                      tinyxml2
                      ${catkin_LIBRARIES}
                      )

target_link_libraries(set-position-limits
                      tinyxml2
                      ${catkin_LIBRARIES}
                      )

target_link_libraries(dynamixel-server
                      tinyxml2
                      ${catkin_LIBRARIES}
                      )

target_link_libraries(test_ass 
                      ${catkin_LIBRARIES}
                      )

## Add cmake target dependencies of the executable/library
## as an example, message headers may need to be generated before nodes

## Specify libraries to link a library or executable target against
# target_link_libraries(ohm_dynamixel_node
#   ${catkin_LIBRARIES}
# )

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

# Mark executables and/or libraries for installation
install(TARGETS dynamixel-server
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

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
# catkin_add_gtest(${PROJECT_NAME}-test test/test_ohm_dynamixel.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
