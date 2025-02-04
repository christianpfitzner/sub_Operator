/*
 * MapperDrive.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#include "MapperDrive.h"
#include "Communication.h"
#include "gui/Hud.h"
#include "MapperController.h"
#include "ohm_actors_msgs/SensorHeadJoy.h"
#include "ohm_teleop_msgs/FlipperAngle.h"

#include "ohm_schroedi_mc/FlipperPreset.h"

#include <std_srvs/Empty.h>

#include <geometry_msgs/Twist.h>

#include "PsProfiles.h"
#include <string>

namespace phros_remote
{

static double _threshSwitchDir = 0.25;

MapperDrive::MapperDrive(ros::NodeHandle& nh, MapperController::ButtonActions* btnActions, MapperController::AxisActions* axisActions)
    : _nh(nh)
    , _homingIters(0)
    , _homingYawFor(0.0)
    , _homingPitchFor(0.0)
{
  std::string topicTwist;
  std::string topicFlippers;
  std::string topicSensorHead;
  double      threshSpeedLinearVar  = 0.0;
  double      threshSpeedAngVar     = 0.0;
  double      threshSpeedSensorHead = 0.0;
  double      speedFlipperManual    = 0.0;

  ros::NodeHandle prvNh("~");
//  prvNh.param<double>("thresh_speed_linear", threshSpeedLinearVar, 1.3);
  prvNh.param<double>("thresh_speed_linear", threshSpeedLinearVar, 0.4);
//  prvNh.param<double>("thresh_speed_linear", threshSpeedAngVar, 0.8);
  prvNh.param<double>("thresh_speed_linear", threshSpeedAngVar, 0.6);
//  prvNh.param<double>("thresh_speed_sensor_head", threshSpeedSensorHead, 0.6);
  prvNh.param<double>("thresh_speed_sensor_head", threshSpeedSensorHead, 0.4);
  prvNh.param<double>("speed_flipper_manual", speedFlipperManual, 2.4);
  prvNh.param<double>("homing_yaw_for", _homingYawFor, 0.0);
  prvNh.param<double>("homing_pitch_for", _homingPitchFor, -0.4);

  _threshSpeedLinear     = static_cast<float>(threshSpeedLinearVar);
  _threshSpeedAngular    = static_cast<float>(threshSpeedAngVar);
  _threshSpeedSensorHead = static_cast<float>(threshSpeedSensorHead);
  _speedFlipperManual    = static_cast<float>(speedFlipperManual);



  _flipper_preset_service = _nh.serviceClient<ohm_schroedi_mc::FlipperPreset>("/flipper/preset"); 
  // ros::Subscriber _flupper_angle_sub = _nh.subscribe<

  _btnActions = btnActions;
  _axisActions = axisActions;
}

MapperDrive::~MapperDrive()
{
  // TODO Auto-generated destructor stub
}

void MapperDrive::map(std::shared_ptr<MapperPsPad>& msg)
{
  auto hud = Hud::getInstance();
  hud->setArmActive(false);
  if(_reset)
  {
    msg->reset();
    _reset = false;
    return;
  }
  if(msg->button(_btnActions->mapperSwitch).edge() == StateButton::Edge::RISING)
  {
    MapperController::getInstance()->switchMapper(IMapper::RemoteType::HUD);
    return;
  }

  ohm_actors_msgs::SensorHeadJoy sensorHead;
  if(_homingIters) // this is kind of a hack. As long as still some "iters" are
                   // left, the homing mode is still active so
  {                // the head will turn itself in the new homing position
    sensorHead.home = true;
    _homingIters--;
  }
  else
    sensorHead.home = false;
  
  //map twist forward

  // todo make this a parameter to be scaled
  geometry_msgs::Twist twist;

  // this creates a squared velocity curve
  float speed_input = msg->axis(_axisActions->moveFwd) - msg->axis(_axisActions->moveBwd);
  float speed_input_normalized = speed_input / 2.0; 
  float speed_input_squared = speed_input_normalized * speed_input_normalized; 

  // recreate the sign of the speed
  if (speed_input < 0.0)
    speed_input_squared *= -1.0; 


  // twist.linear.x = -0.7 * (msg->axis(MapperPsPad::AxesPad::R2) - msg->axis(MapperPsPad::AxesPad::L2)) * _threshSpeedLinear;
  twist.linear.x = -0.55 * speed_input_squared; 

  float angular_input = msg->axis(_axisActions->moveLeftRight); 
  float angular_squared = angular_input * angular_input; 

  if (angular_input < 0.0)
    angular_squared *= -1.0; 


  twist.angular.z = 1.8*angular_squared; 
  
  // map sensor head
  sensorHead.pitch = msg->axis(_axisActions->shPitch) * _threshSpeedSensorHead; 
  sensorHead.yaw   = msg->axis(_axisActions->shYaw) * _threshSpeedSensorHead; 
  if(msg->button(_btnActions->shHome).state()) 
    sensorHead.home = true;

  

  // map flipper presets for compact
  if(msg->button(_btnActions->flipperCompact).state())
  {
    // call service for compact
    ohm_schroedi_mc::FlipperPreset srv; 
    srv.request.command  = 5; 
    srv.request.inverted = 0; 

    _flipper_preset_service.call(srv);

  }

  // set flippers to w shape
  if(msg->button(_btnActions->flipperW).state())
  {
    // call service for w configuration
    ohm_schroedi_mc::FlipperPreset srv; 
    srv.request.command  = 1; 
    srv.request.inverted = 0; 

    _flipper_preset_service.call(srv);
  }


  
  // map flippers
  ohm_teleop_msgs::FlipperAngle flippers;
  double vz = 0.0;
  if(msg->button(_btnActions->flipperUp).state()) 
    vz = 1.0;
  else if(msg->button(_btnActions->flipperDown).state()) // msg->crossDown())
    vz = -1.0;
  if(msg->button(_btnActions->flipperFl).state()) 
    flippers.front_left = -1.0 * _speedFlipperManual * vz;
  if(msg->button(_btnActions->flipperFr).state()) 
    flippers.front_right = -1.0 * _speedFlipperManual * vz;
  if(msg->button(_btnActions->flipperBl).state()) 
    flippers.back_left = _speedFlipperManual * vz;
  if(msg->button(_btnActions->flipperBr).state()) 
    flippers.back_right = _speedFlipperManual * vz;

   
  

  //publish data
  auto comm = Communication::getInstance();
  comm->publishToDrive(twist);
  comm->publishToSensorHead(sensorHead);
  comm->publishToFlippers(flippers);

  static ros::Time timerSwitch = ros::Time::now();
  auto             mapCtrl     = MapperController::getInstance();

  //switch between forward and backward drive
  if((msg->button(_btnActions->driveSwitch1).state()) && (msg->button(_btnActions->driveSwitch2).state()))
  {
    const double timePressed = (ros::Time::now() - timerSwitch).toSec();
    if((timePressed > _threshSwitchDir) && !_switched)
    {
      mapCtrl->switchMapper(IMapper::RemoteType::DRIVE_REV, true);
      return;
      timerSwitch = ros::Time::now();
      _switched   = false;
    }
  }
  else
  {
    timerSwitch = ros::Time::now();
    _switched   = false;
  }
}

void MapperDrive::mapImage(void)
{
  auto hud       = Hud::getInstance();
  auto comm      = Communication::getInstance();
  auto imageMain = comm->imageMain();
  if(imageMain)
  {
    hud->setImageMain(imageMain);
    hud->update();
  }
  else
    ROS_ERROR_STREAM_THROTTLE(1.0, __PRETTY_FUNCTION__ << " no image received");
}

void MapperDrive::init(void)
{
  auto comm = Communication::getInstance();
  comm->setHoming(_homingPitchFor, _homingYawFor);
  _homingIters = 50; // TODO: this is so very dumb. Add a homing service to
                     // sensor head and call it from here
}

} // namespace phros_remote
