/*
 * flipper_control.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: phil
 */


#include <ros/ros.h>

#include "ohm_schroedi_mc/mc_ret.h"
#include "ohm_teleop_msgs/FlipperSpeed.h"
#include "ohm_teleop_msgs/FlipperAngle.h"
#include "ohm_schroedi_mc/FlipperPreset.h"
#include "LocalizationFlipper.h"

#include <string>
#include <cmath>

void callBackMc(const ohm_schroedi_mc::mc_ret& msg);
void callBackFlipSpeed(const ohm_teleop_msgs::FlipperSpeed& msg);
void callBackTimerMain(const ros::TimerEvent& ev);
bool callBackFlipperPreset(ohm_schroedi_mc::FlipperPreset::Request& req, ohm_schroedi_mc::FlipperPreset::Response& res);
double flipperTicksToAngle(const int32_t ticks);


static ros::Publisher _pubFlipperAngles;
static ros::Publisher _pubFlipperAnglePres;
static double _dt = 0.0; ///< discrete time interval for manual flipper change
static ros::Time _callBackMCreceived;
static ros::Time _callBackFlipperReceived;
static ros::Time _callBackPuReceived;
static double _lagTimeOut = 0.0;
static ohm_teleop_msgs::FlipperAngle _angleDes;
static ohm_teleop_msgs::FlipperAngle _anglePres;

//static const double GEAR_R = 6.75 * 216.0;
//static const double TICS_P_TURN = 512.0;

enum class Flippers
{
  FRONT_LEFT = 0,
  FRONT_RIGHT,
  REAR_LEFT,
  REAR_RIGHT
};

static std::vector<LocalizationFlipper*> _flipperLocal(4, NULL);  //todo: nur weil du zu dämlich bist um gscheid zu programmieren!!

int main(int argc, char** argv)
{
  ros::init(argc, argv, "flipper_control");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");
  std::string topicMC;
  std::string topicFlipperSpeed;
  std::string topicFlipperAngleDes;
  std::string topicFlipperAnglePres;
  std::string topicFlipperPreset;
  std::string topicPu;

  prvNh.param<std::string>("topic_mc", topicMC, "mc_ret");
  prvNh.param<std::string>("topic_flipper_speed", topicFlipperSpeed, "flipper/manual");
  prvNh.param<std::string>("topic_flipper_angle_des", topicFlipperAngleDes, "cmd/flip");
  prvNh.param<std::string>("topic_flipper_angle_pres", topicFlipperAnglePres, "flipper/angle/present");
  prvNh.param<std::string>("topic_flipper_preset", topicFlipperPreset, "flipper/preset");
  prvNh.param<std::string>("topic_pu", topicPu, "pu");

  prvNh.param<double>("dt_flip_manual", _dt, 0.1);
  prvNh.param<double>("lag_timout", _lagTimeOut, 0.1);

  ros::Subscriber subsMcRet = nh.subscribe(topicMC, 1, callBackMc);
  ros::Subscriber subsFlipSpeed = nh.subscribe(topicFlipperSpeed, 1, callBackFlipSpeed);
  ros::ServiceServer srvFlipperPreset = nh.advertiseService(topicFlipperPreset, callBackFlipperPreset);
  _pubFlipperAngles = nh.advertise<ohm_teleop_msgs::FlipperAngle>(topicFlipperAngleDes, 1);
  _pubFlipperAnglePres = nh.advertise<ohm_teleop_msgs::FlipperAngle>(topicFlipperAnglePres, 1);

  _flipperLocal[static_cast<unsigned int>(Flippers::FRONT_LEFT)] = new LocalizationFlipper("flipper_front_left", nh);
  _flipperLocal[static_cast<unsigned int>(Flippers::FRONT_RIGHT)] = new LocalizationFlipper("flipper_front_right", nh);
  _flipperLocal[static_cast<unsigned int>(Flippers::REAR_LEFT)] = new LocalizationFlipper("flipper_rear_left", nh);
  _flipperLocal[static_cast<unsigned int>(Flippers::REAR_RIGHT)] = new LocalizationFlipper("flipper_rear_right", nh);

  ros::Timer timerMain = nh.createTimer(ros::Duration(0.01), callBackTimerMain);
  ros::spin();
}

void callBackMc(const ohm_schroedi_mc::mc_ret& msg)
{
  _callBackMCreceived = ros::Time::now();
  _anglePres.front_left  = flipperTicksToAngle(msg.flipper_front_left_ticks_ret);
  _flipperLocal[static_cast<unsigned int>(Flippers::FRONT_LEFT)]->update(msg.flipper_front_left_ticks_ret);
  _anglePres.front_right = flipperTicksToAngle(msg.flipper_front_right_ticks_ret);
  _flipperLocal[static_cast<unsigned int>(Flippers::FRONT_RIGHT)]->update(msg.flipper_front_right_ticks_ret);
  _anglePres.back_left   = flipperTicksToAngle(msg.flipper_rear_left_ticks_ret  );
  _flipperLocal[static_cast<unsigned int>(Flippers::REAR_LEFT)]->update(msg.flipper_rear_left_ticks_ret);
  _anglePres.back_right  = flipperTicksToAngle(msg.flipper_rear_right_ticks_ret );
  _flipperLocal[static_cast<unsigned int>(Flippers::REAR_RIGHT)]->update(msg.flipper_rear_right_ticks_ret);
}

void callBackFlipSpeed(const ohm_teleop_msgs::FlipperSpeed& msg)
{
  _callBackFlipperReceived = ros::Time::now();
  if(std::abs(msg.front_left) > 0.01)
    _angleDes.front_left  = _anglePres.front_left  + _dt * msg.front_left ;
  if(std::abs(msg.front_right) > 0.01)
    _angleDes.front_right = _anglePres.front_right + _dt * msg.front_right;
  if(std::abs(msg.back_left) > 0.01)
    _angleDes.back_left   = _anglePres.back_left   + _dt * msg.back_left  ;
  if(std::abs(msg.back_right) > 0.01)
    _angleDes.back_right  = _anglePres.back_right  + _dt * msg.back_right ;
}

void callBackTimerMain(const ros::TimerEvent& ev)
{
  const bool flipLag = ((ros::Time::now() - _callBackFlipperReceived).toSec() > _lagTimeOut);
  const bool motLag = ((ros::Time::now() - _callBackMCreceived).toSec() > _lagTimeOut);
  //const bool puLag = ((ros::Time::now() - _callBackPuReceived).toSec() > _lagTimeOut);
  //std::cout << __PRETTY_FUNCTION__ << " f m  " << flipLag << " " << motLag << std::endl;

  if(0)//flipLag || motLag)
  {
    std::cout << __PRETTY_FUNCTION__ << " f m " << flipLag << " " << motLag << std::endl;
    ROS_ERROR("%s error. lag detected ", __PRETTY_FUNCTION__);
    return;
  }
  _pubFlipperAnglePres.publish(_anglePres);
  _pubFlipperAngles.publish(_angleDes);
}

double flipperTicksToAngle(const int32_t ticks)
{
  double var = static_cast<double>(ticks);
  return (var / (GEAR_R * TICS_P_TURN)) * 2.0 * M_PI;
}

bool callBackFlipperPreset(ohm_schroedi_mc::FlipperPreset::Request& req, ohm_schroedi_mc::FlipperPreset::Response& res)
{
  //ohm_teleop_msgs::FlipperAngle angleDes;
  _callBackFlipperReceived = ros::Time::now();
  if(req.command == ohm_schroedi_mc::FlipperPreset::Request::ASCEND)
  {
    _angleDes.front_left = -2.57885527611;
    _angleDes.front_right = -2.57885527611;
    _angleDes.back_left = 0.0;
    _angleDes.back_right = 0.0;
  }
  else if(req.command == ohm_schroedi_mc::FlipperPreset::Request::FLAT)
  {
    _angleDes.front_left = -3.43834733963;
    _angleDes.front_right = -3.43834733963;
    _angleDes.back_left = 0.0;
    _angleDes.back_right = 0.0;
  }
  else if(req.command == ohm_schroedi_mc::FlipperPreset::Request::SMALLFPRINT)
  {
    _angleDes.front_left = -M_PI * 0.5;
    _angleDes.front_right = -M_PI * 0.5;
    _angleDes.back_left = M_PI * 0.5;
    _angleDes.back_right = M_PI * 0.5;
  }
  else
    return false;

  //_pubFlipperAngles.publish(angleDes);
  return true;
}
