/*
 * ohm_gripper.cpp
 *
 *  Created on: Mar 27, 2018
 *      Author: phil
 */



#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_srvs/SetBool.h>

#include "ohm_actors_msgs/DriveState.h"

#include <iostream>

void timerCallBack(const ros::TimerEvent& ev);
//void callBackComand(const std_msgs::Bool& cmd); //todo: implement this shit
void callBackDriveStateJoint5  (const ohm_actors_msgs::DriveState& state);
void callBackDriveStateJoint6  (const ohm_actors_msgs::DriveState& state);
void callBackDriveStateJoint7  (const ohm_actors_msgs::DriveState& state);
void callBackDriveStateGripper1(const ohm_actors_msgs::DriveState& state);
void callBackDriveStateGripper2(const ohm_actors_msgs::DriveState& state);

void callBackGripperStep(const std_msgs::Float32& angleStep);
void callBackGripper1Pos(const std_msgs::Float32& angle);
void callBackGripper2Pos(const std_msgs::Float32& angle);


bool callBackHeavyMetal(std_srvs::SetBool::Request& req, std_srvs::SetBool::Response& res);


static ros::Publisher _pubAngleJoint5;
static ros::Publisher _pubAngleJoint6;
static ros::Publisher _pubAngleJoint7;
static ros::Publisher _pubAngleGripper1;
static ros::Publisher _pubAngleGripper2;
static float _angleGripper1 = 0.0;
static float _angleGripper2 = 0.0;
static double _gripperSleepAngle = 0.0;
static double _threshTimeSleep = 0.0;

static ros::Time _sleepTimer;// = ros::Time::now();
static bool _heavyMetalMode = true;

static const float ANGLE_MIN = 0.32;
static const float ANGLE_MAX = 1.0;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "ohm_gripper");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");
  _sleepTimer = ros::Time::now();
  std::string topicStateJoint5;
  std::string topicStateJoint6;
  std::string topicStateJoint7;
  std::string topicStateGripper1;
  std::string topicStateGripper2;

  std::string topicAngleJoint5;
  std::string topicAngleJoint6;
  std::string topicAngleJoint7;
  std::string topicAngleGripper1;
  std::string topicAngleGripper2;

  std::string topicHeavyMetal;

  prvNh.param<std::string>("topic_state_joint5",         topicStateJoint5,   "/joint_5/command");
  prvNh.param<std::string>("topic_state_joint6",         topicStateJoint6,   "/joint_6/command");
  prvNh.param<std::string>("topic_state_joint7",         topicStateJoint7,   "/joint_7/command");
  prvNh.param<std::string>("topic_state_state_gripper1", topicStateGripper1, "/joint_gripper_1/command");
  prvNh.param<std::string>("topic_state_state_gripper2", topicStateGripper2, "/joint_gripper_2/command");
  prvNh.param<std::string>("topic_angle_joint5",         topicAngleJoint5,   "/joint5/pos/des");
  prvNh.param<std::string>("topic_angle_joint6",         topicAngleJoint6,   "/joint6/pos/des");
  prvNh.param<std::string>("topic_angle_joint7",         topicAngleJoint7,   "/joint7/pos/des");
  prvNh.param<std::string>("topic_angle_gripper1",       topicAngleGripper1, "/gripper1/pos/des");
  prvNh.param<std::string>("topic_angle_gripper2",       topicAngleGripper2, "/gripper2/pos/des");
  prvNh.param<std::string>("topic_heavy_metal", topicHeavyMetal, "/heavy_metal_mode");
  prvNh.param<double>("gripper_sleep_angle", _gripperSleepAngle, 0.0);
  prvNh.param<double>("thresh_time_sleep", _threshTimeSleep, 1.0);

  _pubAngleJoint5   = nh.advertise<std_msgs::Float32>(topicAngleJoint5, 1);
  _pubAngleJoint6   = nh.advertise<std_msgs::Float32>(topicAngleJoint6, 1);
  _pubAngleJoint7   = nh.advertise<std_msgs::Float32>(topicAngleJoint7, 1);
  _pubAngleGripper1 = nh.advertise<std_msgs::Float32>(topicAngleGripper1, 1);
  _pubAngleGripper2 = nh.advertise<std_msgs::Float32>(topicAngleGripper2, 1);

  ros::Subscriber subsStateJoint5   = nh.subscribe(topicStateJoint5   ,1, callBackDriveStateJoint5);
  ros::Subscriber subsStateJoint6   = nh.subscribe(topicStateJoint6   ,1, callBackDriveStateJoint6);
  ros::Subscriber subsStateJoint7   = nh.subscribe(topicStateJoint7   ,1, callBackDriveStateJoint7);
  ros::Subscriber subsStateGripper1 = nh.subscribe(topicStateGripper1 ,1, callBackDriveStateGripper1);
  ros::Subscriber subsStateGripper2 = nh.subscribe(topicStateGripper2 ,1, callBackDriveStateGripper2);
  ros::Subscriber subsStep = nh.subscribe("gripper_step", 1, callBackGripperStep);
  ros::Subscriber subsAngleGripper1 = nh.subscribe("gripper1/pos/present", 1, callBackGripper1Pos);
  ros::Subscriber subsAngleGripper2 = nh.subscribe("gripper2/pos/present", 1, callBackGripper2Pos);


  ros::ServiceServer srvHeavyMetal = nh.advertiseService(topicHeavyMetal, callBackHeavyMetal);

 // ros::Timer timer = nh.createTimer(ros::Duration(0.01), timerCallBack);
  ros::spin();
}

void callBackDriveStateJoint5  (const ohm_actors_msgs::DriveState& state)
{
  std_msgs::Float32 msg;
  msg.data = static_cast<float>(state.position);
  _pubAngleJoint5.publish(msg);
}

void callBackDriveStateJoint6  (const ohm_actors_msgs::DriveState& state)
{
  std_msgs::Float32 msg;
  msg.data = static_cast<float>(state.position);
  _pubAngleJoint6.publish(msg);
}

void callBackDriveStateJoint7  (const ohm_actors_msgs::DriveState& state)
{
  std_msgs::Float32 msg;
  msg.data = static_cast<float>(state.position);
  _pubAngleJoint7.publish(msg);
}

void callBackDriveStateGripper1(const ohm_actors_msgs::DriveState& state)
{
  std_msgs::Float32 msg;
  msg.data = static_cast<float>(state.position);
  _pubAngleGripper1.publish(msg);
}

void callBackDriveStateGripper2(const ohm_actors_msgs::DriveState& state)
{
  std_msgs::Float32 msg;
  msg.data = static_cast<float>(state.position);
  _pubAngleGripper2.publish(msg);
}

void callBackGripperStep(const std_msgs::Float32& angleStep)
{
  std_msgs::Float32 command;
  command.data = _angleGripper1 + angleStep.data;
  _pubAngleGripper1.publish(command);
  command.data = _angleGripper2 + (-1.0 * angleStep.data);  //todo: check the sign
  _pubAngleGripper2.publish(command);
}

void callBackGripper1Pos(const std_msgs::Float32& angle)
{
  _angleGripper1 = angle.data;
//  std::cout << __PRETTY_FUNCTION__ << " angle " << angle.data << std::endl;
}

void callBackGripper2Pos(const std_msgs::Float32& angle)
{
  _angleGripper2 = angle.data;
//  std::cout << __PRETTY_FUNCTION__ << " angle " << angle.data << std::endl;
}

void timerCallBack(const ros::TimerEvent& ev)
{
//  if((ros::Time::now() - _sleepTimer).toSec() > _threshTimeSleep)
//  {
//    std_msgs::Float32 msg;
//    msg.data = static_cast<float>(_gripperSleepAngle);
//    _pubAngleJoint5.publish(msg);
//  }
}

bool callBackHeavyMetal(std_srvs::SetBool::Request& req, std_srvs::SetBool::Response& res)
{
  _heavyMetalMode = req.data;
  res.success = _heavyMetalMode;
  std_msgs::Float32 msg;

  if(_heavyMetalMode)
  {
    res.message = "YEah. PervVision Bitches!!!";
    msg.data = static_cast<float>(_gripperSleepAngle);
  }
  else
  {
    res.message = "Still brave";
    msg.data = static_cast<float>(0.0);
  }
  _pubAngleJoint5.publish(msg);
  return true;
}
