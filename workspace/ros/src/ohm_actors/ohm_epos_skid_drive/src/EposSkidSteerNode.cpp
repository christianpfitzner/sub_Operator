/*
 * EposSkidSteerNode.cpp
 *
 *  Created on: Mar 23, 2017
 *      Author: phil
 */

#include "EposSkidSteerNode.h"

#include <nav_msgs/Odometry.h>

#include <string>
#include <cstring>

#define ACCELERATION 1000
#define DECELERATION 2000
#define LEFT_MOTOR_ID 0x01
#define RIGHT_MOTOR_ID 0x02

namespace autonohm
{

EposSkidSteerNode::EposSkidSteerNode()
{
  ros::NodeHandle prvNh("~");

  std::string topicTwist;
  std::string topicOdom;
  std::string tfBaseFrame;
  std::string tfOdomFrame;
  std::string devicePtrMotor;

  prvNh.param<std::string>("topic_twist", topicTwist, "cmd/vel");
  prvNh.param<std::string>("topic_odom",  topicOdom, "odom");
  prvNh.param<std::string>("tf_base_frame", tfBaseFrame, "map");
  prvNh.param<std::string>("tf_odom_frame", tfOdomFrame, "odom");
  prvNh.param<std::string>("device_motor", devicePtrMotor, "/dev/ttyUSB0");

  char deviceArray[devicePtrMotor.size()];
  std::memcpy(deviceArray, devicePtrMotor.c_str(), devicePtrMotor.size() * sizeof(char));

  double TmainLoop = 0.0;

  _timer     = _nh.createTimer(ros::Duration(TmainLoop), &EposSkidSteerNode::callBackMainTimer, this);
  _subsTwist = _nh.subscribe(topicTwist, 1, &EposSkidSteerNode::callBackTwist, this);
  _pubOdom   = _nh.advertise<nav_msgs::Odometry>(topicOdom, 1);

  if(!_motors.InitDevice(deviceArray))
  {
    ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " error! Opening of device ptr " << devicePtrMotor << " failed");
    std::exit(1);
  }
  _motors.setAcceleration(ACCELERATION, LEFT_MOTOR_ID);
  _motors.setAcceleration(ACCELERATION, RIGHT_MOTOR_ID);
  _motors.setDeceleration(DECELERATION, LEFT_MOTOR_ID);
  _motors.setDeceleration(DECELERATION, RIGHT_MOTOR_ID);

  _wheelGauge    = 0.5;
  _wheelDiameter = 0.3;
  _gearRatio     = 3;

  _timer = _nh.createTimer(ros::Duration(1.0 / 50.0), &EposSkidSteerNode::callBackMainTimer, this);

}

EposSkidSteerNode::~EposSkidSteerNode()
{
  // TODO Auto-generated destructor stub
}

void EposSkidSteerNode::start(void)
{
  ros::spin();
}

void EposSkidSteerNode::callBackMainTimer(const ros::TimerEvent& e)
{
  this->generateOdom();
}

void EposSkidSteerNode::callBackTwist(const geometry_msgs::Twist& twist)
{

  //bool DifferentialDrive::move(const geometry_msgs::Twist::ConstPtr& cmd_vel){
  double vr, vl;
      twist_to_tracks(&vl, &vr, twist.linear.x, twist.angular.z);

      double tpt_l = track_to_TicksPerTurn(vl) * 60.0;
      double tpt_r = track_to_TicksPerTurn(vr) * 60.0;


      _motors.setRPM(tpt_l, tpt_r, LEFT_MOTOR_ID, RIGHT_MOTOR_ID);

//  _motors.setRPM((500.0 * twist.linear.x)+(-500.0 * twist.angular.z), (-500.0 * twist.linear.x)+(-500.0 * twist.angular.z)
//      , LEFT_MOTOR_ID, RIGHT_MOTOR_ID);
}

void EposSkidSteerNode::generateOdom(void)
{
  long left = 0;
  long right = 0;
  _motors.getActualVelocity(&left, LEFT_MOTOR_ID);
  _motors.getActualVelocity(&right, RIGHT_MOTOR_ID);
  std::cout << __PRETTY_FUNCTION__ << " l r " << left << " " << right << std::endl;
}

double EposSkidSteerNode::track_to_TicksPerTurn(double v)
{
  return (v / (_wheelDiameter * M_PI)) * _gearRatio;
};

void EposSkidSteerNode::twist_to_tracks(double *vl, double *vr, double v, double omega)
{
  *vr = -1 * (v + omega * _wheelGauge / 2.0); // ToDo: Direction var einführen
  *vl =       v - omega * _wheelGauge / 2.0;
};

} /* namespace autonohm */
