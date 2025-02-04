/*
 * gripper_loop.cpp
 *
 *  Created on: Apr 25, 2019
 *      Author: phil
 */

#include <ros/ros.h>
#include <std_msgs/Float32.h>

#include <dynamic_reconfigure/server.h>
#include "ohm_dynamixel/ReconfigureGripperTestConfig.h"

#include <string>

static ros::Publisher _pubGripperAngle;
static float _gripperOpen   = 0.0;
static float _gripperClosed = 0.0;
static float _gripperCurrent = NAN;
static float _arrivedThresh = 0.0;
static float _angleDesired = 0.0;
static bool _opening = false;
static bool _loop = false;


void timerCallBack(const ros::TimerEvent& ev);
void callBackGripperAngle(const std_msgs::Float32& angle);
void callBackDynamicReconfigure(const ohm_dynamixel::ReconfigureGripperTestConfig& msg, uint32_t level);

int main(int argc, char** argv)
{
  ros::init(argc, argv, "gripper_loop");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");

  std::string topicGripperCur;
  std::string topicGripperDes;

  prvNh.param<std::string>("topic_gripper_des", topicGripperCur, "/gripper/pos/present");
  prvNh.param<std::string>("topic_gripper_cur", topicGripperDes, "/gripper/pos/des");

  _pubGripperAngle = nh.advertise<std_msgs::Float32>(topicGripperDes, 1);
  ros::Subscriber subsGripperAngleCurrent = nh.subscribe(topicGripperCur, 1, callBackGripperAngle);

  dynamic_reconfigure::Server<ohm_dynamixel::ReconfigureGripperTestConfig> server;
  dynamic_reconfigure::Server<ohm_dynamixel::ReconfigureGripperTestConfig>::CallbackType f;

  f = boost::bind(&callBackDynamicReconfigure, _1, _2);
  server.setCallback(f);

  ros::Timer timer = nh.createTimer(ros::Duration(0.01), timerCallBack);

  ros::spin();

}

void timerCallBack(const ros::TimerEvent& ev)
{
  if(std::isnan(_gripperCurrent))
  {
    ROS_INFO_STREAM(__PRETTY_FUNCTION__ << " Warning. No pose received yet" << std::endl);
    return;
  }
  std_msgs::Float32 gripperDes;
  if(_loop)
  {
    if(_opening)
    {
      // std::cout << __PRETTY_FUNCTION__ << "opening " <<_gripperOpen  << " - " << _gripperCurrent << " = " << std::abs(_gripperOpen - _gripperCurrent) << " < " << _arrivedThresh <<std::endl;
      if(std::abs(_gripperOpen - _gripperCurrent) < _arrivedThresh)
      {
        // std::cout << __PRETTY_FUNCTION__ << " opened. Switch to closing" << std::endl;
        gripperDes.data = _gripperClosed;
        _opening = false;
      }
      else
        gripperDes.data = _gripperOpen;
    }
    else
    {
      //  std::cout << __PRETTY_FUNCTION__ << "closing " <<_gripperClosed  << " - " << _gripperCurrent << " = " << std::abs(_gripperClosed - _gripperCurrent) << " < " <<  _arrivedThresh <<std::endl;
      if(std::abs(_gripperClosed - _gripperCurrent) < _arrivedThresh)
      {
        //  std::cout << __PRETTY_FUNCTION__ << " closed. Switch to opening" << std::endl;
        gripperDes.data = _gripperOpen;
        _opening = true;
      }
      else
        gripperDes.data = _gripperClosed;
    }
  }
  else
    gripperDes.data = _angleDesired;
  _pubGripperAngle.publish(gripperDes);
}

void callBackGripperAngle(const std_msgs::Float32& angle)
{
  _gripperCurrent = angle.data;
}

void callBackDynamicReconfigure(const ohm_dynamixel::ReconfigureGripperTestConfig& msg, uint32_t level)
{
  _gripperOpen = msg.open_position;
  _gripperClosed = msg.closed_position;
  _arrivedThresh = msg.arrived_thresh;
  _loop = msg.loop_on;
  _angleDesired = msg.des_angle;

}
