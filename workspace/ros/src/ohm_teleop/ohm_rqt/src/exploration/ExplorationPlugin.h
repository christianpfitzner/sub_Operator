/*
 * ExplorationPlugin.h
 *
 *  Created on: May 4, 2019
 *      Author: jasmin
 */

#ifndef OHM_RQT_SRC_EXPLORATION_EXPLORATIONPLUGIN_H_
#define OHM_RQT_SRC_EXPLORATION_EXPLORATIONPLUGIN_H_

#include <rqt_gui_cpp/plugin.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include "ui_exploration.h"
#include "std_srvs/SetBool.h"

namespace exploration
{

class ExplorationPlugin: public rqt_gui_cpp::Plugin
{
	Q_OBJECT

public:
	ExplorationPlugin();
	virtual ~ExplorationPlugin();
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);
	virtual void shutdownPlugin();
	virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const;
	virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);

public slots:	//slots=subscriber - signal=publisher
  void button_start_pause(bool checked);
  void button_stop(bool checked);

private:
	QWidget* _widgetMain;
	Ui::exploration* _gui;
	ros::ServiceClient _client;
	ros::NodeHandle _nh;
	std_srvs::SetBool _srvSetBool;
};

} /* namespace co2 */

#endif /* OHM_RQT_SRC_EXPLORATION_EXPLORATIONPLUGIN_H_ */
