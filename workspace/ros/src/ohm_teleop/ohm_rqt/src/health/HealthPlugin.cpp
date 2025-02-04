/*
 * HealthPlugin.cpp
 *
 *  Created on: Mar 26, 2018
 *      Author: juicy
 */

#include "HealthPlugin.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>
#include <QDebug>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sstream>
#include <iostream>

namespace my_health
{

HealthPlugin::HealthPlugin():
  rqt_gui_cpp::Plugin(),
  _widgetMain(NULL),
  _gui(NULL)
{
  // give QObjects reasonable names
  setObjectName("HealthPlugin");
  _subspu = n.subscribe("pu", 1, &HealthPlugin::callback, this); //this weil subscriber zur klasse healthplugin gehört, stimmt?
}


HealthPlugin::~HealthPlugin()
{
  // TODO Auto-generated destructor stub
}


void HealthPlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
  // access standalone command line arguments
  QStringList argv = context.argv();
  // present UI file
  _gui = new Ui::health_plugin;
  // create QWidget
  _widgetMain = new QWidget();
  // extend the widget with all attributes and children from UI file
  _gui->setupUi(_widgetMain);
  // add widget to the user interface
  context.addWidget(_widgetMain);
}

void HealthPlugin::shutdownPlugin()
{
  // TODO unregister all publishers here
  _subspu.shutdown();
  ros::shutdown();
}

void HealthPlugin::callback(const ohm_rqt::pu& msg)
{
//  double val = 0.1;
//  std::stringstream ss;
//  ss << val;
//  std::string strVal= ss.str();
//  std::string str = "Hello world";
//  QString qstr = QString::fromStdString(str);

  float fiveC = msg.c_5V;
  //std::stringstream fiveCss;
  //fiveCss << fiveC;
  //std::string fiveCstring = fiveCss.str();
  //QString QStringfiveC = QString::fromStdString(fiveCstring);
  //QString abcd = "abcd";

  //std::cout << fiveCstring.size() << std::endl;
  //std::cout << fiveCstring << std::endl;
  //std::cout << fiveC << std::endl;

  //QString fiveCString = QString::number(fiveC);

  //QString text_5c = QString ("fiveLine c: %1").arg(msg.c_5V);
  //_gui->fiveLine_c->setFormat(text_5c);

  _gui->fiveLine_c->setValue(fiveC);

  //_gui->label5c->setText(QStringfiveC);

  //_gui->label5c->setNum(msg.c_5V);

  //QString text = QString( "%p% (%1 KB/s)" ).arg( speedInKbps );
  //progressBar->setFormat( text );

  //_gui->fiveLine_c->setFormat("Hallo i bims 80%");

  _gui->fiveLine_r->setValue(msg.r_5V);
  _gui->twelveLine_c->setValue(msg.c_12V);
  _gui->twelveLine_r->setValue(msg.r_12V);
  _gui->twentyfourLine_c->setValue(msg.c_24V);
  _gui->twentyfourLine_r->setValue(msg.r_24V);
  _gui->fourtyeightLine_c->setValue(msg.c_48V);
  _gui->fourtyeightLine_r->setValue(msg.r_48V);
  _gui->lcdNumber24->display(msg.alarm_24V);
  //_gui->lcdNumber24->setSmallDecimalPoint(true);
  _gui->lcdNumber48->display(msg.alarm_48V);
}



void HealthPlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const
{
  // TODO save intrinsic configuration, usually using:
  // instance_settings.setValue(k, v)
}

void HealthPlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{
  // TODO restore intrinsic configuration, usually using:
  // v = instance_settings.value(k)
}

/*bool hasConfiguration() const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used                                to open a dialog to offer the user a set of configuration
}*/

} /* namespace ohm_pu */
PLUGINLIB_EXPORT_CLASS(my_health::HealthPlugin, rqt_gui_cpp::Plugin)




