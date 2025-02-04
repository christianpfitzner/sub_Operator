/*
 * CO2Plugin.cpp
 *
 *  Created on: May 3, 2019
 *      Author: jasmin
 */

#include "CO2Plugin.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>
#include <QDebug>
#include <ros/ros.h>
#include <QObject>
#include <iostream>

namespace co2
{

CO2Plugin::CO2Plugin():
		rqt_gui_cpp::Plugin(),
		_widgetMain(NULL),
		_gui(NULL)
{
   setObjectName("CO2Plugin");
   _subCO2 = _nh.subscribe("co2sensor_node/co2Concentration", 1, &CO2Plugin::co2Callback, this);

}


CO2Plugin::~CO2Plugin()
{
	// TODO Auto-generated destructor stub
}

void CO2Plugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
   QStringList argv = context.argv();
   _gui = new Ui::co2;
   _widgetMain = new QWidget();
   _gui->setupUi(_widgetMain);
   context.addWidget(_widgetMain);
}


void CO2Plugin::shutdownPlugin()
{
   _subCO2.shutdown();
   ros::shutdown();
}
void CO2Plugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const
{

}
void CO2Plugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{

}

void CO2Plugin::co2Callback(const std_msgs::Int64& value)
{
   int intValue = value.data;
   //std::cout << "hallo intValue = " << intValue << std::endl;
   //_gui->progressBar_co2->setValue(_co2Value.data);
   //_gui->label_nbr->setValue(_co2Value.data);
   _gui->label_nbr->setNum(intValue);

}

} /* namespace co2 */
PLUGINLIB_EXPORT_CLASS(co2::CO2Plugin, rqt_gui_cpp::Plugin)

