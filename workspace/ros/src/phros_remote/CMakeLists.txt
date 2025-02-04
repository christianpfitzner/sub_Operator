cmake_minimum_required(VERSION 2.8.3)
project(phros_remote)

find_package(catkin REQUIRED COMPONENTS
  std_msgs
  message_generation
  roscpp
  rqt_gui
  rqt_gui_cpp
  image_transport
  ohm_teleop_msgs
  ohm_actors_msgs
  ohm_schroedi_mc   #this dependency should be changed. Move the schroedi mc messages to the actors msgs
  ohm_power_unit    #this dependency should be changed. Move the power unit messages to the actors msgs
  ohm_visca_control
)


set(CMAKE_AUTORCC ON)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})
find_package(OpenCV REQUIRED)
find_package(cmake_modules REQUIRED)
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS} "include/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++17 ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -fPIC -mtune=native")

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
################################################
## Declare ROS messages, services and actions ##
################################################

#add_message_files(
#	FILES
#	pu.msg
#)

#generate_messages(
#	DEPENDENCIES
#	std_msgs
#)



###################################
## catkin specific configuration ##
###################################

catkin_package(
  INCLUDE_DIRS ${INC_DIR}
  LIBRARIES ${PROJECT_NAME}
  ${CMAKE_CURRENT_BINARY_DIR}/..
  CATKIN_DEPENDS roscpp rqt_gui rqt_gui_cpp std_msgs message_runtime message_generation
)
#catkin_package(
#  CATKIN_DEPENDS roscpp  
#)


###########
## Build ##
###########


SET(UIS_REMOTE src/gui/main.ui
              src/gui/dialog_warning_armreset.ui)

SET(HDRS_REMOTE src/RemotePlugin.h
				src/gui/WidgetMain.h
        src/gui/Hud.h
        src/gui/DialogWarningArmReset.h)

SET(SRCS_REMOTE src/RemotePlugin.cpp
				src/gui/WidgetMain.cpp
				src/Communication.cpp
				src/gui/Hud.cpp
        src/gui/PerspectiveModel.cpp
        src/gui/DialogWarningArmReset.cpp
        src/utils/SuperImageSubscriber.cpp
        src/utils/MapperPs4Pad.cpp
        src/utils/MapperPsPad.cpp
        src/utils/MapperPsPadSing.cpp
        src/utils/StateButton.cpp
        src/utils/MapperPs3Pad.cpp
				src/MapperController.cpp
				src/mappers/MapperBase.cpp
				src/mappers/MapperArm.cpp
				src/mappers/MapperDrive.cpp
				src/mappers/MapperDriveReverse.cpp
        src/mappers/MapperHud.cpp
        src/mappers/MapperInspect.cpp
				src/gui/menu/MenuItemBase.cpp
				src/gui/menu/MenuItemModeArm.cpp
				src/gui/menu/MenuItemModeDrive.cpp
				src/gui/menu/MenuItemModeDriveReverse.cpp
        src/gui/menu/MenuItemModeInspect.cpp
        src/gui/menu/MenuItemFlipperAscent.cpp
        src/gui/menu/MenuItemFlipperDescent.cpp
				src/gui/menu/MenuItemFlipperFlat.cpp
        src/gui/menu/MenuItemFlipperUp4.cpp
        src/gui/menu/MenuItemArmInitialIdle.cpp
        src/gui/menu/MenuItemArmInitialLowered.cpp
        src/gui/menu/MenuItemArmInitialLookDown.cpp
        src/gui/menu/MenuItemArmInitialLookSide.cpp
        src/gui/menu/MenuItemArmInitialLookUp.cpp
				src/gui/icons/qt_res.qrc)

SET(HDRS_MENU_TEST src/MenuTest.h 
                   src/ThreadRos.h
                   #src/RemotePlugin.h
				src/gui/WidgetMain.h
        src/gui/Hud.h
        src/gui/DialogWarningArmReset.h
                   )
SET(SRCS_MENU_TEST src/menu_test.cpp 
                   src/MenuTest.cpp 
                   src/gui/menu/Menu.cpp
                   src/ThreadRos.cpp
                   src/gui/WidgetMain.cpp
				src/Communication.cpp
				src/gui/Hud.cpp
        src/gui/PerspectiveModel.cpp
        src/gui/DialogWarningArmReset.cpp
        src/utils/SuperImageSubscriber.cpp
        src/utils/MapperPs4Pad.cpp
        src/utils/MapperPsPad.cpp
        src/utils/MapperPsPadSing.cpp
        src/utils/StateButton.cpp
        src/utils/MapperPs3Pad.cpp
				src/MapperController.cpp
				src/mappers/MapperBase.cpp
				src/mappers/MapperArm.cpp
				src/mappers/MapperDrive.cpp
				src/mappers/MapperDriveReverse.cpp
        src/mappers/MapperHud.cpp
        src/mappers/MapperInspect.cpp
				src/gui/menu/MenuItemBase.cpp
				src/gui/menu/MenuItemModeArm.cpp
				src/gui/menu/MenuItemModeDrive.cpp
				src/gui/menu/MenuItemModeDriveReverse.cpp
        src/gui/menu/MenuItemModeInspect.cpp
        src/gui/menu/MenuItemFlipperAscent.cpp
        src/gui/menu/MenuItemFlipperDescent.cpp
				src/gui/menu/MenuItemFlipperFlat.cpp
        src/gui/menu/MenuItemFlipperUp4.cpp
        src/gui/menu/MenuItemArmInitialIdle.cpp
        src/gui/menu/MenuItemArmInitialLowered.cpp
        src/gui/menu/MenuItemArmInitialLookDown.cpp
        src/gui/menu/MenuItemArmInitialLookSide.cpp
        src/gui/menu/MenuItemArmInitialLookUp.cpp
				src/gui/icons/qt_res.qrc
                   )


##########################################################################################

set(INC_DIR
  include
   ${PROJECT_SOURCE_DIR}/src/gui
   ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
)

include_directories(${INC_DIR} ${catkin_INCLUDE_DIRS})

##########################################################################################
# qt5_wrap_cpp produces moc files for all headers listed
# qt5_wrap_ui produces .h files for all .ui files listed

## Remote:
qt5_wrap_cpp(MOCS_SRCS_REMOTE ${HDRS_REMOTE})
qt5_wrap_ui(UI_HEADER_REMOTE ${UIS_REMOTE})

qt5_wrap_cpp(MOCS_SRCS_MENU_TEST ${HDRS_MENU_TEST})

##########################################################################################

add_library(phros_remote ${SRCS_REMOTE} ${MOCS_SRCS_REMOTE} ${UI_HEADER_REMOTE})
target_link_libraries(phros_remote ${catkin_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})



add_executable(menu_test ${SRCS_MENU_TEST} ${MOCS_SRCS_MENU_TEST} ${UI_HEADER_REMOTE})
target_link_libraries(menu_test 
PUBLIC
${catkin_LIBRARIES} 
Qt5::Core
Qt5::Gui 
Qt5::Widgets)
#set_property(TARGET menu_test PROPERTY POSITION_INDEPENDENT_CODE ON)