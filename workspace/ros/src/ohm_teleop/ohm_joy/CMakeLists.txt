cmake_minimum_required(VERSION 2.8.3)



project(ohm_joy)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
    roscpp
    message_generation
    ohm_actors_msgs
    ohm_teleop_msgs
    )

#changed qt4 to 5
#find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
#added
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${EIGEN_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../build/ohm_joy
		    )


set(QT_HEADERS src/JoyConfig.h
               src/JoyConfigInputWidget.h
               )

set(QT_FORMS
               src/ui/JoyConfig.ui
               src/ui/JoyConfigInputWidget.ui
                )

set(QT_SOURCES
               src/JoyConfig.cpp
               src/JoyConfigInputWidget.cpp
                )
#changed qt4 to 5
QT5_WRAP_CPP(QT_MOC_HPP ${QT_HEADERS}
                )

QT5_WRAP_UI( QT_FORMS_HPP ${QT_FORMS}
                )


## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependencies might have been
##     pulled in transitively but can be declared for certainty nonetheless:
##     * add a build_depend tag for "message_generation"
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
#add_message_files(
#   FILES
#   Joy_Action.msg
#)

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
# generate_messages(
#   DEPENDENCIES
#   std_msgs  # Or other packages containing msgs
# )

#generate_messages()


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
#  LIBRARIES ohm_joy
  CATKIN_DEPENDS ohm_actors_msgs ohm_teleop_msgs
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
# add_library(ohm_joy
#   src/${PROJECT_NAME}/ohm_joy.cpp
# )

## Declare a cpp executable
add_executable(ohm_joy_config
                            src/ohm_joy_config.cpp
                            ${QT_SOURCES}
                            ${QT_MOC_HPP}
                            ${QT_FORMS_HPP}
                            )

add_executable(ohm_joy_node
                            src/ohm_joy_node.cpp
                            src/JoyMapper.cpp
                            ${QT_SOURCES}
                            ${QT_MOC_HPP}
                            ${QT_FORMS_HPP}
                            )
                            
add_executable(ohm_joy_advanced
                            src/ohm_joy_advanced_node.cpp
                            src/JoyMapper.cpp
                            src/plugins/JoyToVel.cpp
                            src/plugins/JoyToFlipper.cpp
                            src/plugins/JoyToManipulator.cpp
                            src/plugins/JoyToSensorhead.cpp
                            ${QT_SOURCES}
                            ${QT_MOC_HPP}
                            ${QT_FORMS_HPP}
                            )

add_executable(ohm_joy_rough_node src/ohm_joy_rough_node.cpp)

## Add cmake target dependencies of the executable/library
## as an example, message headers may need to be generated before nodes
add_dependencies(ohm_joy_config     ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(ohm_joy_node       ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(ohm_joy_advanced   ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
 target_link_libraries(ohm_joy_config
   ${catkin_LIBRARIES}
#   ${QT_LIBRARIES}
   Qt5::Core Qt5::Widgets Qt5::Gui
 )

 target_link_libraries(ohm_joy_node
   ${catkin_LIBRARIES}
   ${QT_LIBRARIES}
   Qt5::Core Qt5::Widgets Qt5::Gui
 )
 
 target_link_libraries(ohm_joy_advanced
   ${catkin_LIBRARIES}
   ${QT_LIBRARIES}
   Qt5::Core Qt5::Widgets Qt5::Gui
 )

target_link_libraries(ohm_joy_rough_node
   ${catkin_LIBRARIES}
   )
   


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
install(TARGETS ohm_joy_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

file(GLOB launch_files "launch/*.launch"
                        )

## Mark other files for installation (e.g. launch and bag files, etc.)
install(FILES
    ${launch_files}
#   # myfile1
#   # myfile2
   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)



## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )



#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_ohm_joy.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
