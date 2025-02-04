/*
 * TiltScanPlugin.h
 *
 *  Created on: September 29, 20221
 *      Author: marco
 */

#ifndef OHM_RQT_SRC_TITL_SCAN_TiltScanPlugin_H_
#define OHM_RQT_SRC_TITL_SCAN_TiltScanPlugin_H_

#include <rqt_gui_cpp/plugin.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <ros/ros.h>
#include <ohm_sensors_msgs/StartTiltScan3D.h>
#include "ui_tiltscan.h"


namespace tiltscan
{

class TiltScanPlugin: public rqt_gui_cpp::Plugin
{
	Q_OBJECT

public:
	TiltScanPlugin();
	virtual ~TiltScanPlugin();
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);
	virtual void shutdownPlugin();
	virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const;
	virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);

public slots:	//slots=subscriber - signal=publisher
  void button_scan(bool checked);

private:
	QWidget* _widgetMain;
	Ui::tiltscan* _gui;
	ros::ServiceClient _srvTiltScan;
	ros::NodeHandle _nh;
};

} /* namespace tiltscan */

#endif /* OHM_RQT_SRC_TITL_SCAN_TiltScanPlugin_H_ */
