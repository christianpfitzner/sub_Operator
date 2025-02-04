/**************************************************************\
 * Authors: Yannick Wallerer
 *          Rainer Walther
 *          Philipp Weiß
 *
 * Date:    11.02.2019
 *
 * Description: This plugin contains a subscriber waiting for
 *              messages of type sensor_msgs::Imu.
 *              From this message, the needed angles pitch and
 *              roll can be derived. The display of
 *              WidgetHorizon will be set accordingly.
\**************************************************************/


#ifndef my_horizon__horizon_H
#define my_horizon__horizon_H

#include <rqt_gui_cpp/plugin.h>
#include <QtWidgets/QWidget>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>


namespace my_horizon {

class horizon : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  horizon();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const;
  virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);
  void setAngle(const sensor_msgs::Imu& msg);
public slots:

  // Comment in to signal that the plugin has a way to configure it
  //bool hasConfiguration() const;
  //void triggerConfiguration();

private:
  QWidget* widget_;
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
};
} // namespace
#endif // my_namespace__my_plugin_H
