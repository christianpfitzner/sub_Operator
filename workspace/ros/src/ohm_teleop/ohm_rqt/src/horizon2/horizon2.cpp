#include "horizon2.h"

#include <pluginlib/class_list_macros.h>
#include <QtCore/QStringList>
#include <std_msgs/String.h>
#include <ros/ros.h>
#include <tf/LinearMath/Matrix3x3.h>
#include <tf/LinearMath/Scalar.h>
#include "WidgetHorizon2.h"

namespace my_horizon2 {

horizon2::horizon2():rqt_gui_cpp::Plugin(),
    widget_(0)
{
  setObjectName("horizon");
}

void horizon2::initPlugin(qt_gui_cpp::PluginContext& context)
{
  QStringList argv = context.argv();
  widget_ = new WidgetHorizon2();
  context.addWidget(widget_);
  sub_ = nh_.subscribe("/imu/data", 1, &horizon2::setAngle, this);
}

void horizon2::shutdownPlugin()
{
  // TODO unregister all publishers here
}

void horizon2::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const
{
  // TODO save intrinsic configuration, usually using:
  // instance_settings.setValue(k, v)
}

void horizon2::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
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
void horizon2::setAngle(const sensor_msgs::Imu& msg)
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
  dynamic_cast<WidgetHorizon2*>(widget_)->setPitch(pitch);
  dynamic_cast<WidgetHorizon2*>(widget_)->setRoll(roll);

  widget_->update();
}

} // namespace
PLUGINLIB_EXPORT_CLASS(my_horizon2::horizon2, rqt_gui_cpp::Plugin)
