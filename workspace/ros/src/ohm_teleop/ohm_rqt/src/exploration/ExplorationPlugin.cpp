/*
 * ExplorationPlugin.cpp
 *
 *  Created on: May 4, 2019
 *      Author: jasmin
 */

#include "ExplorationPlugin.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>
#include <QDebug>
#include <ros/ros.h>
#include <QObject>
#include <iostream>
#include <stdio.h>

namespace exploration
{

ExplorationPlugin::ExplorationPlugin():
   		rqt_gui_cpp::Plugin(),
   		_widgetMain(NULL),
   		_gui(NULL)
   {
      setObjectName("ExplorationPlugin");
   }

ExplorationPlugin::~ExplorationPlugin() {
	// TODO Auto-generated destructor stub
}

void ExplorationPlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
   QStringList argv = context.argv();
   _gui = new Ui::exploration;
   _widgetMain = new QWidget();
   _gui->setupUi(_widgetMain);
   context.addWidget(_widgetMain);

   _client = _nh.serviceClient<std_srvs::SetBool>("startStopExploration");

   connect(_gui->pushButton_startpause, SIGNAL(clicked(bool)), this, SLOT(button_start_pause(bool)));
   connect(_gui->pushButton_stop, SIGNAL(clicked(bool)), this, SLOT(button_stop(bool)));

}
void ExplorationPlugin::shutdownPlugin()
{
   _client.shutdown();
   ros::shutdown();
}
void ExplorationPlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const
{

}
void ExplorationPlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{

}

void ExplorationPlugin::button_start_pause(bool checked)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	std::cout << "Button clicked" << std::endl;

	_srvSetBool.request.data = true;

	if(_client.call(_srvSetBool))
	{
	  ROS_INFO("State was changed: %s -> message: %s", _srvSetBool.response.success ? "true" : "false", _srvSetBool.response.message.c_str());
	}
	else
	{
	  ROS_ERROR("Failed to call service Start/Stop Exploration");
	}

	std::cout << "_srvSetBool.response = " << _srvSetBool.response.message << std::endl;
}

void ExplorationPlugin::button_stop(bool checked)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	std::cout << "Button clicked" << std::endl;

	_srvSetBool.request.data = false;

	if(_client.call(_srvSetBool))
	{
	  ROS_INFO("State was changed: %s -> message: %s", _srvSetBool.response.success ? "true" : "false", _srvSetBool.response.message.c_str());
	}
	else
	{
	  ROS_ERROR("Failed to call service Start/Stop Exploration");
	}
	std::cout << "_srvSetBool.response = " << _srvSetBool.response << std::endl;
}

} /* namespace co2 */
PLUGINLIB_EXPORT_CLASS(exploration::ExplorationPlugin, rqt_gui_cpp::Plugin)
