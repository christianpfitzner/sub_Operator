/*
 * HealthPlugin.h
 *
 *  Created on: Mar 26, 2018
 *      Author: juicy
 */

#ifndef OHM_POWER_UNIT_SRC_HEALTHPLUGIN_H_
#define OHM_POWER_UNIT_SRC_HEALTHPLUGIN_H_


#include <rqt_gui_cpp/plugin.h>
//#include "ohm_power_unit/ui_health_plugin.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include "ui_health_plugin.h"
#include "ohm_rqt/pu.h"

namespace my_health
{

class HealthPlugin : public rqt_gui_cpp::Plugin
{
	Q_OBJECT
public:
	HealthPlugin();
	virtual ~HealthPlugin();
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);
	virtual void shutdownPlugin();
	virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const;
	virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);
	void callback(const ohm_rqt::pu& msg);
private:
	QWidget* _widgetMain;
	Ui::health_plugin* _gui;
	ros::Subscriber _subspu;
	ros::NodeHandle n;
};

} /* namespace my_health */

#endif /* OHM_POWER_UNIT_SRC_HEALTHPLUGIN_H_ */
