/*
 * RqtPluginGripper.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: phil
 */

#include "RqtPluginGripper.h"

#include <pluginlib/class_list_macros.h>

#include <QPushButton>

namespace ohm_gripper
{

RqtPluginGripper::RqtPluginGripper():
    _widgetMain(NULL),
    _guiUi(NULL),
    _as("gripper_action", false)
{


}

RqtPluginGripper::~RqtPluginGripper()
{
  // TODO Auto-generated destructor stub
}

void RqtPluginGripper::initPlugin(qt_gui_cpp::PluginContext& context)
{
  _widgetMain = new QWidget();
  _guiUi = new Ui::GuiGripper;
  _guiUi->setupUi(_widgetMain);
  std::cout << __PRETTY_FUNCTION__ << " waiting for action server" << std::endl;
  if(!_as.waitForServer(ros::Duration(1.0)))
  {
    std::cout << __PRETTY_FUNCTION__ << " Action server is not responding " << std::endl;
    ros::shutdown();
    std::exit(1);
  }
  connect(_guiUi->pushButtonGrip, SIGNAL(clicked()), this, SLOT(grip()));
  connect(_guiUi->pushButtonOpen, SIGNAL(clicked()), this, SLOT(open()));
  context.addWidget(_widgetMain);
}

void RqtPluginGripper::shutdownPlugin()
{
  //_as.
}

void RqtPluginGripper::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const
{

}

void RqtPluginGripper::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{

}

void RqtPluginGripper::grip(void)
{
  std::cout << __PRETTY_FUNCTION__ << " grip command " << std::endl;
  ohm_actors_msgs::ActionGripperGoal goal;
  goal.cmd = ohm_actors_msgs::ActionGripperGoal::CLOSE;
  _as.sendGoal(goal);
}

void RqtPluginGripper::open(void)
{
  std::cout << __PRETTY_FUNCTION__ << " open command " << std::endl;
  ohm_actors_msgs::ActionGripperGoal goal;
    goal.cmd = ohm_actors_msgs::ActionGripperGoal::OPEN;
    _as.sendGoal(goal);
}

void RqtPluginGripper::stepClose(void)
{

}

void RqtPluginGripper::stepOpen(void)
{

}

} /* namespace ohm_gripper */
PLUGINLIB_DECLARE_CLASS(ohm_gripper, RqtPluginGripper, ohm_gripper::RqtPluginGripper, rqt_gui_cpp::Plugin)
