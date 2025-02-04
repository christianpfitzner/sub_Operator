/*
 * TiltScanPlugin.h
 *
 *  Created on: September 29, 20221
 *      Author: marco
 */

#include "TiltScanPlugin.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>
#include <QDebug>
#include <ros/ros.h>
#include <QObject>
#include <iostream>

namespace tiltscan
{

TiltScanPlugin::TiltScanPlugin():
		rqt_gui_cpp::Plugin(),
		_widgetMain(NULL),
		_gui(NULL)
{
   setObjectName("TiltScanPlugin");
}


TiltScanPlugin::~TiltScanPlugin()
{
	// TODO Auto-generated destructor stub
}

void TiltScanPlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
   QStringList argv = context.argv();
   _gui = new Ui::tiltscan;
   _widgetMain = new QWidget();
   _gui->setupUi(_widgetMain);
   context.addWidget(_widgetMain);


   _srvTiltScan = _nh.serviceClient<ohm_sensors_msgs::StartTiltScan3D>("scanparamsSrv");
   connect(_gui->pushButton_scan, SIGNAL(clicked(bool)), this, SLOT(button_scan(bool)));
}


void TiltScanPlugin::shutdownPlugin()
{
   ros::shutdown();
}
void TiltScanPlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const
{

}
void TiltScanPlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{

}

void TiltScanPlugin::button_scan(bool checked)
{
	ohm_sensors_msgs::StartTiltScan3D srv;
   srv.request.speed = 30;
	srv.request.startPosition = 90;
	srv.request.endPosition = 180;

	if(!_srvTiltScan.call(srv))
	{
	  ROS_ERROR("Failed to call Scan service");
	}
}

} /* namespace tiltscan */
PLUGINLIB_EXPORT_CLASS(tiltscan::TiltScanPlugin, rqt_gui_cpp::Plugin)

