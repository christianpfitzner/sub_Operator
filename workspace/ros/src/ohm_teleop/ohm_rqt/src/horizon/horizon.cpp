#include "horizon.h"
#include <pluginlib/class_list_macros.h>
#include <QtCore/QStringList>
#include <std_msgs/String.h>
#include <ros/ros.h>
#include "WidgetHorizon.h"
#include <tf/LinearMath/Matrix3x3.h>
#include <tf/LinearMath/Scalar.h>

namespace my_horizon {

horizon::horizon():rqt_gui_cpp::Plugin(),
    widget_(0)
{
  setObjectName("horizon");
}

void horizon::initPlugin(qt_gui_cpp::PluginContext& context)
{
  QStringList argv = context.argv();
  widget_ = new WidgetHorizon();
  context.addWidget(widget_);
  sub_ = nh_.subscribe("/imu/data", 1, &horizon::setAngle, this);
}

void horizon::shutdownPlugin()
{
  // TODO unregister all publishers here
}

void horizon::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const
{
  // TODO save intrinsic configuration, usually using:
  // instance_settings.setValue(k, v)
}

void horizon::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{
  // TODO restore intrinsic configuration, usually using:
  // v = instance_settings.value(k)
}

/*bool hasConfiguration() //const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used to open a dialog to offer the user a set of configuration
}*/


// Callback function which is executed when another node publishes a message on the topic "angle".
// For correct functionality, the message has to be of the type sensor_msgs::Imu
void horizon::setAngle(const sensor_msgs::Imu& msg)
{
  tfScalar roll, pitch, yaw;
  tf::Quaternion quat(msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w);
  tf::Matrix3x3 mat(quat);

  mat.getRPY(roll, pitch, yaw, 1);
  
  // invert yaw, otherwise schrödi is a spaceship
  tf::Matrix3x3 inv;
  inv.setRPY(0,0,-yaw);
  mat = inv * mat;
  mat.getRPY(roll, pitch, yaw);

  // widget_ is a pointer of the type QWidget*
  // As setPitch and setRoll are members of WidgetHorizon, a dynamic cast is required
  dynamic_cast<WidgetHorizon*>(widget_)->setPitch(pitch);
  dynamic_cast<WidgetHorizon*>(widget_)->setRoll(roll);

  widget_->update();
}

} // namespace
PLUGINLIB_EXPORT_CLASS(my_horizon::horizon, rqt_gui_cpp::Plugin)
