/*
 * LocalizationFlipper.cpp
 *
 *  Created on: Apr 13, 2018
 *      Author: phil
 */

#include "LocalizationFlipper.h"

#include <std_msgs/Float32.h>

LocalizationFlipper::LocalizationFlipper(const std::string& base, ros::NodeHandle& nh):
_nh(nh)
{
  ros::NodeHandle prvNh("~");
  prvNh.param<std::string>(base + "/frame_base", _frameBase, base + "/base");
  prvNh.param<std::string>(base + "/frame_child", _frameChild, base + "/dyn");
  _pubAnglePresent = _nh.advertise<std_msgs::Float32>(base + "/angle/present", 1);
}

LocalizationFlipper::~LocalizationFlipper()
{
  // TODO Auto-generated destructor stub
}

void LocalizationFlipper::update(const int32_t& ticksPresent)
{
  const double anglePresent = this->flipperTicksToAngle(ticksPresent);
  std_msgs::Float32 msg;
  msg.data = static_cast<float>(anglePresent);
  _pubAnglePresent.publish(msg);

//  tf::StampedTransform tf;
//  tf.frame_id_ = _frameBase;
//  tf.child_frame_id_ = _frameChild;
//  tf::Quaternion quat;
//  quat.setRPY(0.0, anglePresent, 0.0);
//  tf.setRotation(quat);
//  tf.setOrigin(tf::Vector3(0.0, 0.0, 0.0));
//  _bc.sendTransform(tf);
}

double LocalizationFlipper::flipperTicksToAngle(const int32_t& ticks)
{
  double var = static_cast<double>(ticks);
  return (var / (GEAR_R * TICS_P_TURN)) * 2.0 * M_PI;
}
