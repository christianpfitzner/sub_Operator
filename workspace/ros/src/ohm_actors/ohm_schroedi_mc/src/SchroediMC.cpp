/*
 * SchroediMC.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: phil
 */

#include "SchroediMC.h"
#include "ohm_schroedi_mc/VelRps.h"
#include <string>
#include <ros/ros.h>
#include <ros/console.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose2D.h>
#include <cmath>
#include <tf/tf.h>

static const float FLP_GEAR_R = 6.75 * 216.0;
static const float FLP_T_P_TURN = 512.0;
static const float TRACK_GEAR_R = 5.0 * 36.0;  //wormgear * planetarygear
static const float TRACK_DIAMETER = 0.159;
//static const float ROBOT_DIAMETER = 0.90;
//static const float ROBOT_DIAMETER = 0.535;
static const float ROBOT_DIAMETER = 0.50;
static const float FUCK_FAC = 1.00;

#define RESET_TRACK_DRIVES 0b1000000000000000
#define REFERENCE_TRACK_DRIVES 0b0000000000000001

SchroediMC::SchroediMC():
        _flagDriveCtrl(false)
{
  std::string topicTwist;
  std::string topicFlipper;
  std::string topicCtrl;
  std::string topicFlipperPowerLess;
  std::string topicRPSReturn;
  std::string topicOdom;
  std::string topicPu;
  std::string topicDriveCtrl;
  double loopRate = 0.0;
  ros::NodeHandle prvNh("~");

  prvNh.param<std::string>("topic_velocity", topicTwist, "vel/cmd");
  prvNh.param<std::string>("topic_flipper", topicFlipper, "cmd/flip");
  prvNh.param<std::string>("topic_ctrl", topicCtrl, "cmd/mc_ctrl");
  prvNh.param<std::string>("topic_flipper_powerless", topicFlipperPowerLess, "flipper_powerless");
  prvNh.param<std::string>("topic_rps_return", topicRPSReturn, "mc_ret");
  prvNh.param<std::string>("topic_odom", topicOdom, "odom");
  prvNh.param<std::string>("topic_pu", topicPu, "pu");
  prvNh.param<std::string>("topic_drive_ctrl", topicDriveCtrl, "drive_control");

  prvNh.param<double>("loop_rate", loopRate, 20.0);
  prvNh.param<double>("time_out_lag", _timeOutLag, 0.5);
  prvNh.param<double>("v_max", _vMax, 1.0);
  prvNh.param<double>("track", _track, 1.0);
  prvNh.param<double>("wheel_base", _wheelBase, 1.0);
  prvNh.param<double>("gear_ratio", _gearRatio, 1.0);
  prvNh.param<double>("wheel_circumference", _wheelCircumference, 1.0);
  prvNh.param<double>("wheel_base", _wheelBase, 1.0);
  prvNh.param<double>("gear_ratio", _gearRatio, 1.0);
  prvNh.param<double>("diagonal", _diagonal, 1.0);
  prvNh.param<double>("cos_a", _cosa, 1.0);
  prvNh.param<int>("thresh_rps_max", _threshRpsMax, 16000);
  prvNh.param<double>("thresh_es", _threshES, 20.0);
  _timerLagPu = ros::Time::now();
  _esActive = true;


  _timerMain = _nh.createTimer(loopRate, &SchroediMC::callbackMainTimer, this);
  _timerLagChain = ros::Time::now();
  _timerLagFlipper = ros::Time::now();
  _subsVel = _nh.subscribe(topicTwist, 1, &SchroediMC::callBackVel, this);
  _subsFlipper = _nh.subscribe(topicFlipper, 1, &SchroediMC::callBackFlip, this);
  _subsPu = _nh.subscribe(topicPu, 1, &SchroediMC::callBackPu, this);
  _subsRPS = _nh.subscribe("mc_ret", 1, &SchroediMC::callBackReturnRpm, this);
  _pubVelMotor = _nh.advertise<ohm_schroedi_mc::mc_ctrl>(topicCtrl, 1);
  _pubOdom = _nh.advertise<nav_msgs::Odometry>(topicOdom, 1);
  _serviceFlipperPowerless   = _nh.advertiseService(topicFlipperPowerLess, &SchroediMC::callBackFlipperPowerLess, this);
  _serviceResetDrives = _nh.advertiseService(topicDriveCtrl, &SchroediMC::callBackDriveCtrl, this);
  _frameRpsMotor.track_left  = 0;
  _frameRpsMotor.track_right = 0;
  _frameRpsMotor.flipper_front_right = 0.0;
  _frameRpsMotor.flipper_front_left  = 0.0;
  _frameRpsMotor.flipper_rear_right  = 0.0;
  _frameRpsMotor.flipper_rear_left   = 0.0;
  _rpmReceived = true;
}

SchroediMC::~SchroediMC()
{
  // TODO Auto-generated destructor stub
}

void SchroediMC::callbackMainTimer(const ros::TimerEvent&)
{
  if((ros::Time::now() - _timerLagChain).toSec() > _timeOutLag)
  {
    this->stopMotors();
  }
  if((ros::Time::now() - _timerLagFlipper).toSec() > _timeOutLag)
    this->stopFlippers();

  if((ros::Time::now() - _timerLagPu).toSec() > _timeOutLag)
  {
    this->stopMotors();
    this->stopFlippers();
    ROS_INFO_THROTTLE(1.0, "power unit lag");
  }
  if(_pu.r_48V < _threshES)
    _esActive = true;
  else
    _esActive = false;

  if(_esActive)
  {
    ROS_INFO_THROTTLE(1.0, "Emergency stop active");
    this->stopMotors();
    this->stopFlippers();
  }
  _pubVelMotor.publish(_frameRpsMotor);
  if(_flagDriveCtrl)
  {
    _frameRpsMotor.control = 0;
    _flagDriveCtrl = false;
  }
//  if(_rpsReceived)
//    _rpsReceived = false;
}

void SchroediMC::callBackVel(const geometry_msgs::Twist& cmd)
{
  double vl = 0.0;
  double vr = 0.0;

  this->twistToTrackspeed(&vl, &vr, cmd.linear.x, cmd.angular.z);
  _frameRpsMotor.track_left =  static_cast<int>(vl);// / 22;// * 36.0; //todo: magic numbers?!
  _frameRpsMotor.track_right = static_cast<int>(vr);// / 22;// * 36.0;
  //std::cout << __PRETTY_FUNCTION__ << " l r " << _frameRpsMotor.track_left << " " << _frameRpsMotor.track_right << std::endl;
  if(std::abs(_frameRpsMotor.track_left) > _threshRpsMax)
  {
    _frameRpsMotor.track_left = _threshRpsMax;
    if(vl < 0.0)
      _frameRpsMotor.track_left *= -1;
  }  

  if(std::abs(_frameRpsMotor.track_right) > _threshRpsMax)
  {
    _frameRpsMotor.track_right = _threshRpsMax;
    if(vr < 0.0)  
      _frameRpsMotor.track_right *= -1;
  }
  // std::cout << __PRETTY_FUNCTION__ << "l r (after rest) " << _frameRpsMotor.track_left << " " << _frameRpsMotor.track_right << std::endl;
  _timerLagChain = ros::Time::now();
}

void SchroediMC::callBackFlip(const ohm_teleop_msgs::FlipperAngle& cmd)
{
  _timerLagFlipper = ros::Time::now();
  if(_esActive)
  {
    return;
  }
  _frameRpsMotor.flipper_front_left  = this->angleToFlipperTicks(cmd.front_left);// static_cast<int16>(cmd.front_left);
  _frameRpsMotor.flipper_front_right = this->angleToFlipperTicks(cmd.front_right);// static_cast<int16>(cmd.front_right);
  _frameRpsMotor.flipper_rear_left   = this->angleToFlipperTicks(cmd.back_left);// static_cast<int16>(cmd.back_left);
  _frameRpsMotor.flipper_rear_right  = this->angleToFlipperTicks(cmd.back_right);// static_cast<int16>(cmd.back_right);
}

void SchroediMC::callBackPu(const ohm_power_unit::pu& pu)
{
  _pu = pu;
  _timerLagPu = ros::Time::now();
}

bool SchroediMC::callBackFlipperPowerLess(std_srvs::SetBool::Request& req, std_srvs::SetBool::Response& res)
{
  if(req.data)
  {
    _frameRpsMotor.control = SchroediMC::FLIP_PWRLESS;
    res.success = true;
  }
  else
  {
    _frameRpsMotor.control = SchroediMC::FLIP_PWR;
    res.success = false;
  }
  return true;
}

void SchroediMC::twistToTrackspeed(double* vl, double* vr, double v, double omega)const  //todo: clean up this mess
{
  std::cout << __PRETTY_FUNCTION__ << "in l r " << v << " " << v << std::endl;
  omega = 0.0;
  *vr = (v + omega * (0.36 / 2.0)); // ToDo: Direction var einführen
  *vl = (v - omega * (0.36 / 2.0));
  std::cout << __PRETTY_FUNCTION__ << "after omega l r " << omega << " " << " " << *vl << " " << *vr <<std::endl;

/*  *vr = -1.0 *  (v - omega * ROBOT_DIAMETER / (2.0 * _cosa));
  *vl =  -1.0 * (v + omega * ROBOT_DIAMETER / (2.0 * _cosa));*/
  *vr /= (TRACK_DIAMETER * M_PI);
  *vl /= (TRACK_DIAMETER * M_PI);
  std::cout << __PRETTY_FUNCTION__ << " after diameter l r " << *vl << " " << *vr <<std::endl;

  *vr *= TRACK_GEAR_R;
  *vl *= TRACK_GEAR_R;

  std::cout << __PRETTY_FUNCTION__ << " after gear l r " << *vl << " " << *vr <<std::endl;
//  *vl *= 18.0;
//  *vr *= 18.0;*/
  *vl *= 60.0 * FUCK_FAC; //rps to rpm
  *vr *= 60.0 * FUCK_FAC; //rps to rpm
  std::cout << __PRETTY_FUNCTION__ << " after rps rpm l r " << *vl << " " << *vr <<std::endl;
}

void SchroediMC::trackspeedToTwist(const double vl, const double vr, double* const v, double* const omega)const
{
  *v     = (vl - vr) / 2.0;
  *omega = (vr + vl) * _cosa / (2.0 * _diagonal);
}

double SchroediMC::trackspeedToTicksPerTurn(double v)const
{
  return (v / _wheelCircumference) * _gearRatio;
}

void SchroediMC::stopMotors(void)
{
  ROS_ERROR_THROTTLE(1.0, " motor lag detected");
  _frameRpsMotor.track_left =          0;//static_cast<int16>(0.0);
  _frameRpsMotor.track_right =         0;//static_cast<int16>(0.0);
}

void SchroediMC::stopFlippers(void)
{
  ROS_ERROR_THROTTLE(1.0, " flipper lag detected -> Action required!");
  //_frameRpsMotor.flipper_front_left =   0;//static_cast<int16>(0.0);
  //_frameRpsMotor.flipper_front_right =  0;//static_cast<int16>(0.0);
  //_frameRpsMotor.flipper_rear_left =    0;//static_cast<int16>(0.0);
  //_frameRpsMotor.flipper_rear_right =   0;//static_cast<int16>(0.0);
}

int32_t SchroediMC::angleToFlipperTicks(const float angle)
{
  float var = (angle * FLP_GEAR_R * FLP_T_P_TURN) / (2.0 * M_PI);
  return static_cast<int32_t>(round(var));
}

bool SchroediMC::callBackDriveCtrl(ohm_schroedi_mc::DriveCtrl::Request& req, ohm_schroedi_mc::DriveCtrl::Response& res)
{
  ohm_schroedi_mc::mc_ctrl ctrl;
  if(req.command == ohm_schroedi_mc::DriveCtrl::Request::REFERENCE)
  {
    std::cout << __PRETTY_FUNCTION__ << " reference drives " << std::endl;
    ctrl.control = REFERENCE_TRACK_DRIVES;
  }
  else if(req.command == ohm_schroedi_mc::DriveCtrl::Request::RESET)
  {
    std::cout << __PRETTY_FUNCTION__ << " reset drives " << std::endl;
    ctrl.control = RESET_TRACK_DRIVES;
  }
  else
  {
    std::cout << __PRETTY_FUNCTION__ << " error! Command " <<static_cast<unsigned int>(req.command) << " not found"  << std::endl;
    return false;
  }
  _frameRpsMotor.control = ctrl.control;
  _flagDriveCtrl = true;
  return true;
}

void SchroediMC::callBackReturnRpm(const ohm_schroedi_mc::mc_ret& rpm)
{
  _rpmCur      = rpm;
  _rpmReceived = true;
  this->calcOdom(rpm);
}

void SchroediMC::calcOdom(const ohm_schroedi_mc::mc_ret& rpm)
{
  static geometry_msgs::Pose2D pose;

  static ros::Time last_time = ros::Time::now();
  ros::Time current_time = ros::Time::now();

  const int32_t rpmL = rpm.track_left_rpm_ret;
  const int32_t rpmR = rpm.track_right_rpm_ret;

  const float vR = static_cast<float>(rpmL) / (60.0 * TRACK_GEAR_R * FUCK_FAC) * TRACK_DIAMETER * M_PI;
  const float vL = static_cast<float>(rpmR) / (60.0 * TRACK_GEAR_R * FUCK_FAC) * TRACK_DIAMETER * M_PI;

  geometry_msgs::Twist twist;
  this->tracksToTwist(vL, vR, &twist);

  double dt = (current_time - last_time).toSec();
  const double angleStep = twist.angular.z * dt;
  const double linStep = twist.linear.x * dt;

  //double dth = twist.angular.z * dt;
  pose.theta += angleStep;
  double dx = linStep * cos(angleStep);
  double dy = linStep * sin(angleStep);
  pose.x     += dx;
  pose.y     += dy;


  nav_msgs::Odometry odom;
  odom.header.stamp = current_time;
  odom.header.frame_id = "odom";
  odom.child_frame_id = "base_footprint";
  odom.pose.pose.position.x = pose.x;
  odom.pose.pose.position.y = pose.y;
  tf::Quaternion quat;
  quat.setRPY(0.0, 0.0, pose.theta);
  odom.pose.pose.orientation.x = quat.x();
  odom.pose.pose.orientation.y = quat.y();
  odom.pose.pose.orientation.z = quat.z();
  odom.pose.pose.orientation.w = quat.w();
  odom.twist.twist = twist;
  odom.pose.covariance[0]      = 1e-8;  // acc x  //todo: something
  odom.pose.covariance[7]      = 1e-8;  // acc y
  odom.pose.covariance[14]     = 9999;  // acc z
  odom.pose.covariance[21]     = 9999;  // roll
  odom.pose.covariance[28]     = 9999;  // pitch
  odom.pose.covariance[35]     = 1e-2;  // yaw
  _pubOdom.publish(odom);
  last_time = current_time;
}

void SchroediMC::tracksToTwist(const double vl, const double vr, geometry_msgs::Twist* const twist)
{
  twist->linear.x  = (vl + vr) / 2.0;
//  twist->angular.z = (vr - vl) / TRACK_DIAMETER;
//twist->angular.z = (vl - vr) / TRACK_DIAMETER;
twist->angular.z = (vl - vr) / ROBOT_DIAMETER;
  std::cout << "tracksToTwist: vl: " << vl << ", vr: " << vr << ", twist->linear.x: "<< twist->linear.x << ", twist->angular.z: " << twist->angular.z << ", TRACK_DIAMETER: " << TRACK_DIAMETER << std::endl;
}
