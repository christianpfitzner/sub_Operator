cmake_minimum_required(VERSION 2.8.3)
project(ohm_robotic_remote)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  image_transport
  nav_msgs
  ohm_perception_msgs
  ohm_teleop_msgs
  roscpp
  sensor_msgs
  tf
  cv_bridge
)

## System dependencies are found with CMake's conventions
find_package(cmake_modules REQUIRED)
find_package(Qt4 REQUIRED)
find_package(VTK REQUIRED)
find_package(Eigen REQUIRED)
find_package(OpenCV REQUIRED)

set(QT_USE_QTXML TRUE)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp -Wall -O2 -std=c++11")


include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

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
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

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
#   nav_msgs#   sensor_msgs
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
#  INCLUDE_DIRS include
#  LIBRARIES ohm_robotic_remote
  CATKIN_DEPENDS image_transport nav_msgs ohm_perception_msgs roscpp sensor_msgs tf ohm_teleop_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(FORM_DIR ${PROJECT_SOURCE_DIR}/forms)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

include_directories(${catkin_INCLUDE_DIRS}
                    ${SRC_DIR}
                    ${PROJECT_SOURCE_DIR}/src/custom-widget
                    ${VTK_INCLUDE_DIRS}
                    ${Eigen_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    /home
                    )

link_directories(#${PROJECT_SOURCE_DIR}/lib/i386
                 ${PROJECT_SOURCE_DIR}/lib/x86_64
                 ${VTK_INCLUDE_DIRS}
                 )

set(OHMGUI_HEADERS src/ImageWidget.h
                   src/MainWindow.h
                   src/ThermoWidget.h
                   src/custom-widget/GamePadButton.h
                   src/GamePadWidget.h
                   src/JoyStickInterface.h

                   src/RobotStatusWidget.h
                   src/custom-widget/DistanceWidget.h
                   src/custom-widget/UltrasoundWidget.h
                   src/custom-widget/ListWidget.h
                   src/custom-widget/ScrollBar.h
                   src/KeyEventBroadcaster.h
                   src/custom-widget/FlatDialog.h
                   src/SensorTestDialog.h
                   ${SRC_DIR}/NodePlayStopWidget.h
                   ${SRC_DIR}/custom-widget/HorizontalSlider.h
                   ${SRC_DIR}/custom-widget/Button.h
                   ${SRC_DIR}/custom-widget/TextSpinBox.h
                   ${SRC_DIR}/custom-widget/IntSpinBox.h
                   ${SRC_DIR}/custom-widget/AbstractButton.h
                   ${SRC_DIR}/custom-widget/AbstractSpinBox.h
                   ${SRC_DIR}/custom-widget/CloudWidget.h
                   ${SRC_DIR}/custom-widget/AutonomousRobotNotification.h
                   )

set(OHMGUI_FORMS forms/MainWindow.ui
                 ${FORM_DIR}/ThermoWidget.ui
                 forms/GamePadWidget.ui
                 src/custom-widget/FlatDialog.ui
                 ${FORM_DIR}/Co2Widget.ui
                 ${FORM_DIR}/AbstractSpinBox.ui
                 ${FORM_DIR}/AutonomousRobotNotification.ui
                 forms/SensorTestDialog.ui
                 )

SET(OHMGUI_RESOURCES ${PROJECT_SOURCE_DIR}/src/picture/resource.qrc )

QT4_WRAP_CPP(OHMGUI_HEADERS_MOC ${OHMGUI_HEADERS})
QT4_WRAP_UI(OHMGUI_FORMS_MOC ${OHMGUI_FORMS})
QT4_ADD_RESOURCES(OHMGUI_RESOURCES_RCC ${OHMGUI_RESOURCES})

add_executable(orr ${SRC_DIR}/main.cpp
                            ${SRC_DIR}/ImageWidget.cpp
                            src/MainWindow.cpp
                            ${SRC_DIR}/ThermoWidget.cpp
                            src/custom-widget/GamePadButton.cpp
                            src/GamePadWidget.cpp
                            src/custom-widget/DistanceWidget.cpp
                            src/custom-widget/UltrasoundWidget.cpp
                            src/RobotStatusWidget.cpp
                            src/KeyEventBroadcaster.cpp
                            src/custom-widget/FlatDialog.cpp
                            src/custom-widget/ListWidget.cpp
                            src/custom-widget/ListWidgetItem.cpp
                            src/custom-widget/ListWidgetModel.cpp
                            src/custom-widget/ScrollBar.cpp
                            src/GetPosition.cpp
                            src/MapWidget.cpp
                            src/Quaternion.cpp
                            src/JoyStickInterface.cpp
                            src/SensorTestDialog.cpp

                            ${SRC_DIR}/Co2Widget.cpp
                            ${SRC_DIR}/custom-widget/HorizontalSlider.cpp
                            ${SRC_DIR}/StarTrekLayout.cpp
                            ${SRC_DIR}/custom-widget/Button.cpp
                            ${SRC_DIR}/custom-widget/TextSpinBox.cpp
                            ${SRC_DIR}/custom-widget/IntSpinBox.cpp
                            ${SRC_DIR}/custom-widget/AbstractSpinBox.cpp
                            ${SRC_DIR}/custom-widget/CloudWidget.cpp
                            ${SRC_DIR}/NodePlayStopWidget.cpp
                            ${SRC_DIR}/VictimClassifierWidget.cpp
                            ${SRC_DIR}/GetPosition.cpp
                            ${SRC_DIR}/custom-widget/AutonomousRobotNotification.cpp

                            ${OHMGUI_HEADERS_MOC}
                            ${OHMGUI_FORMS_MOC}
                            ${OHMGUI_RESOURCES_RCC}
                            )

add_dependencies(orr ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

TARGET_LINK_LIBRARIES(orr
                      ${catkin_LIBRARIES}
                      ${VTK_LIBRARIES}
                      QVTK
                      ${QT_LIBRARIES}
                      PIImager
                      ImageProcessing
                      ${OpenCV_LIBRARIES}
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
install(TARGETS orr
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
# catkin_add_gtest(${PROJECT_NAME}-test test/test_ohm_robotic_remote.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
