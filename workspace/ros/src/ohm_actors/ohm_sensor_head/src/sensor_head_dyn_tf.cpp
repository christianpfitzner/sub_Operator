/*
 * sensor_head_dyn_tf.cpp
 *
 *  Created on: Oct 20, 2018
 *      Author: phil
 */

#include <ros/ros.h>
#include <string>

#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <std_msgs/Float32.h>

void callBackAngleTilt(const std_msgs::Float32& angle);
void callBackAnglePan(const std_msgs::Float32& angle);
void callBackTimerMain(const ros::TimerEvent& event);

static std_msgs::Float32 _anglePan;
static std_msgs::Float32 _angleTilt;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "sensor_head_dyn_tf");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");
  ros::Subscriber subsAngleTilt = nh.subscribe("/tilt/pos/present", 1, callBackAngleTilt);
  ros::Subscriber subsAnglePan = nh.subscribe("/pan/pos/present", 1, callBackAnglePan);
  ros::Timer timerMain = nh.createTimer(ros::Duration(0.01), callBackTimerMain);

  ros::spin();
}

void callBackAngleTilt(const std_msgs::Float32& angle)
{
  _angleTilt = angle;
}

void callBackAnglePan(const std_msgs::Float32& angle)
{
  _anglePan = angle;
}

void callBackTimerMain(const ros::TimerEvent& event)
{
  static tf::TransformBroadcaster bc;
  tf::Transform transform;
  tf::Quaternion rotation;

  tf::Vector3 vec(0.0, 0.0, 0.0);
  transform.setOrigin(vec);
  rotation.setRPY(0.0, 0.0, _anglePan.data);
  transform.setRotation(rotation);
  bc.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "sensor_head/yaw/base", "sensor_head/yaw/rotated"));

  rotation.setRPY(0.0, -_angleTilt.data, 0.0);
  transform.setRotation(rotation);
  bc.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "sensor_head/pitch/base", "sensor_head/pitch/rotated"));
}
