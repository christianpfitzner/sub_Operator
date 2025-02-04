/*
 * CO2Plugin.h
 *
 *  Created on: May 3, 2019
 *      Author: jasmin
 */

#ifndef OHM_RQT_SRC_CO2_CO2PLUGIN_H_
#define OHM_RQT_SRC_CO2_CO2PLUGIN_H_

#include <rqt_gui_cpp/plugin.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <ros/ros.h>
#include <std_msgs/Int64.h>
#include "ui_co2.h"


namespace co2
{

class CO2Plugin: public rqt_gui_cpp::Plugin
{
	Q_OBJECT

public:
	CO2Plugin();
	virtual ~CO2Plugin();
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);
	virtual void shutdownPlugin();
	virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const;
	virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);
	void co2Callback(const std_msgs::Int64& value);

private:
	QWidget* _widgetMain;
	Ui::co2* _gui;
	ros::Subscriber _subCO2;
	ros::NodeHandle _nh;
	//std_msgs::Int64 _co2Value;	//brauch ich gar nit
};

} /* namespace co2 */

#endif /* OHM_RQT_SRC_CO2_CO2PLUGIN_H_ */
