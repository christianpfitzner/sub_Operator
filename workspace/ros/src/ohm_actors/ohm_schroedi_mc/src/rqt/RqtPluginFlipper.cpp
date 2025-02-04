/*
 * RqtPluginFlipper.cpp
 *
 *  Created on: Apr 7, 2018
 *      Author: phil
 */

#include "RqtPluginFlipper.h"

#include <pluginlib/class_list_macros.h>
#include <QDial>

#include "ohm_teleop_msgs/FlipperAngle.h"

namespace ohm_flipper
{

RqtPluginFlipper::RqtPluginFlipper():
  _widgetMain(NULL),
  _ctrlGuiui(NULL)
{
  ros::NodeHandle prvNh("~");
  prvNh.param<std::string>("topic_flipper", _topicFlipper, "cmd/flip");
}

RqtPluginFlipper::~RqtPluginFlipper()
{
  delete _ctrlGuiui;
  delete _widgetMain;
}

void RqtPluginFlipper::initPlugin(qt_gui_cpp::PluginContext& context)
{
  _widgetMain = new QWidget();
  _ctrlGuiui = new Ui::FlipperControl;
  _ctrlGuiui->setupUi(_widgetMain);
  _pubFlipperMessage = _nh.advertise<ohm_teleop_msgs::FlipperAngle>(_topicFlipper, 1);
 connect(_ctrlGuiui->dialFlipFrontLeft,  SIGNAL(valueChanged(int)) , this, SLOT(dialChanged(int)));
 connect(_ctrlGuiui->dialFlipFrontRight, SIGNAL(valueChanged(int)), this, SLOT(dialChanged(int)));
 connect(_ctrlGuiui->dialFlipRearLeft,  SIGNAL(valueChanged(int)) , this, SLOT(dialChanged(int)));
 connect(_ctrlGuiui->dialFlipRearRight,  SIGNAL(valueChanged(int)) , this, SLOT(dialChanged(int)));
  context.addWidget(_widgetMain);
}

void RqtPluginFlipper::shutdownPlugin()
{
  _pubFlipperMessage.shutdown();
  ros::shutdown();
}

void RqtPluginFlipper::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const
{

}

void RqtPluginFlipper::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{

}

void RqtPluginFlipper::dialChanged(int value)
{
  const int flipFrontLeft  = _ctrlGuiui->dialFlipFrontLeft->sliderPosition();
  const int flipFrontRight = _ctrlGuiui->dialFlipFrontRight->sliderPosition();
  const int fliprearLeft   = _ctrlGuiui->dialFlipRearLeft->sliderPosition();
  const int fliprearRight  = _ctrlGuiui->dialFlipRearRight->sliderPosition();

  ohm_teleop_msgs::FlipperAngle flipper;
  flipper.front_left  = static_cast<float>(flipFrontLeft );
  flipper.front_right = static_cast<float>(flipFrontRight);
  flipper.back_left   = static_cast<float>(fliprearLeft  );
  flipper.back_right  = static_cast<float>(fliprearRight );

  _pubFlipperMessage.publish(flipper);

}

}
//PLUGINLIB_DECLARE_CLASS(ohm_flipper, RqtPluginFlipper, ohm_flipper::RqtPluginFlipper, rqt_gui_cpp::Plugin)	
PLUGINLIB_EXPORT_CLASS(ohm_flipper::RqtPluginFlipper, rqt_gui_cpp::Plugin)
