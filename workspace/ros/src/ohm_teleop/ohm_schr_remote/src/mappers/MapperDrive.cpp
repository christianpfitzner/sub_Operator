/*
 * MapperDrive.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#include "MapperDrive.h"
#include "ohm_teleop_msgs/FlipperAngle.h"
#include "ohm_actors_msgs/SensorHeadJoy.h"
#include <std_srvs/Empty.h>

#include <geometry_msgs/Twist.h>

#include <string>
#include "Ps3Profiles.h"

MapperDrive::MapperDrive(ros::NodeHandle& nh):
_nh(nh),
_homingIters(0)
{
  std::string topicTwist;
  std::string topicFlippers;
  std::string topicSensorHead;
  double threshSpeedLinearVar  = 0.0;
  double threshSpeedAngVar     = 0.0;
  double threshSpeedSensorHead = 0.0;
  double speedFlipperManual    = 0.0;

  ros::NodeHandle prvNh("~");

  prvNh.param<std::string>("topic_twist", topicTwist, "cmd/vel");
  prvNh.param<std::string>("topic_flippers", topicFlippers, "flipper/manual");
  prvNh.param<std::string>("topic_sensor_head", topicSensorHead, "sensorhead_joy");
  prvNh.param<double>("thresh_speed_linear", threshSpeedLinearVar,       1.3);
  prvNh.param<double>("thresh_speed_linear", threshSpeedAngVar,          0.8);
  prvNh.param<double>("thresh_speed_sensor_head", threshSpeedSensorHead, 0.6);
  prvNh.param<double>("speed_flipper_manual", speedFlipperManual, 1.7);

  _threshSpeedLinear     = static_cast<float>(threshSpeedLinearVar );
  _threshSpeedAngular    = static_cast<float>(threshSpeedAngVar    );
  _threshSpeedSensorHead = static_cast<float>(threshSpeedSensorHead);
  _speedFlipperManual    = static_cast<float>(speedFlipperManual);

  _pubTwist = _nh.advertise<geometry_msgs::Twist>(topicTwist, 1);
  _pubFlippers = _nh.advertise<ohm_teleop_msgs::FlipperAngle>(topicFlippers, 1);
  _pubSensorHead = _nh.advertise<ohm_actors_msgs::SensorHeadJoy>(topicSensorHead, 1);
  _clientMap = _nh.serviceClient<std_srvs::Empty>("map_victim");
 // _timerSwitchDriveMode = ros::Time::now();
}

MapperDrive::~MapperDrive()
{
  // TODO Auto-generated destructor stub
}

void MapperDrive::map(const sensor_msgs::Joy& joy)
{
  ohm_actors_msgs::SensorHeadJoy sensorHead;
  if(_homingIters)   //this is kind of a hack. As long as still some "iters" are left, the homing mode is still active so
  {                  //the head will turn itself in the new homing position
    sensorHead.home = true;
    _homingIters--;
  }
  geometry_msgs::Twist twist;
  twist.linear.x =  -0.5 * (joy.axes[R2] - joy.axes[L2]) * _threshSpeedLinear;

  twist.angular.z =  joy.axes[A1_X];
  //map sensor head

  sensorHead.pitch = joy.axes[A2_Y] * _threshSpeedSensorHead;
  sensorHead.yaw   = joy.axes[A2_X] * _threshSpeedSensorHead;
  if(joy.buttons[B_A2])
    sensorHead.home = true;
  //map flippers
  ohm_teleop_msgs::FlipperAngle flippers;

  double vz = 0.0;
  if(joy.buttons[B_UP])
    vz = 1.0;
  else if(joy.buttons[B_DOWN])
    vz = -1.0;
  //read select button
  if(joy.buttons[B_T])
    flippers.front_left  = -1.0 * _speedFlipperManual * vz;
  if(joy.buttons[B_C])
    flippers.front_right =  -1.0 * _speedFlipperManual * vz;
  if(joy.buttons[B_S])
    flippers.back_left   = _speedFlipperManual * vz;
  if(joy.buttons[B_X])
    flippers.back_right  = _speedFlipperManual * vz;

  _pubTwist.publish(twist);
  _pubSensorHead.publish(sensorHead);
  _pubFlippers.publish(flippers);
}
