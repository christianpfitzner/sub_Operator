/*
 * MapperDriveReverse.cpp
 *
 *  Created on: Apr 26, 2018
 *      Author: phil
 */

#include "MapperDriveReverse.h"
#include "MapperController.h"

#include "ohm_actors_msgs/SensorHeadJoy.h"
#include "ohm_teleop_msgs/FlipperAngle.h"

#include <geometry_msgs/Twist.h>

#include "Hud.h"
#include "PsProfiles.h"
#include <string>

#include "Communication.h"
#include "Hud.h"

namespace phros_remote
{

static double _threshSwitchDir = 0.25;

MapperDriveReverse::MapperDriveReverse(ros::NodeHandle& nh, MapperController::ButtonActions* btnActions, MapperController::AxisActions* axisActions)
    : _nh(nh)
    , _homingIters(0)
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
  prvNh.param<double>("homing_yaw_rev", _homingYawRev, 2.7);
  prvNh.param<double>("homing_pitch_rev", _homingPitchRev, -0.4);

  _threshSpeedLinear     = static_cast<float>(threshSpeedLinearVar);
  _threshSpeedAngular    = static_cast<float>(threshSpeedAngVar);
  _threshSpeedSensorHead = static_cast<float>(threshSpeedSensorHead);
  _speedFlipperManual    = static_cast<float>(speedFlipperManual);

  _btnActions = btnActions;
  _axisActions = axisActions;
}

MapperDriveReverse::~MapperDriveReverse()
{
  // TODO Auto-generated destructor stub
}

void MapperDriveReverse::map(std::shared_ptr<MapperPsPad>& msg)
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
  if(_homingIters)
  {
    sensorHead.home = true; // this is kind of a hack. As long as still some "iters" are left, the homing mode is still active so
    _homingIters--;         // the head will turn itself in the new homing position
  }
  else
    sensorHead.home = false;
  geometry_msgs::Twist twist;

  // twist.linear.x = 0.5 * (msg->axis(MapperPsPad::AxesPad::R2) - msg->axis(MapperPsPad::AxesPad::L2)) * _threshSpeedLinear;
  // this creates a squared velocity curve
  float speed_input = msg->axis(_axisActions->moveFwd) - msg->axis(_axisActions->moveBwd);
  float speed_input_normalized = speed_input / 2.0; 
  float speed_input_squared = speed_input_normalized * speed_input_normalized; 

  // recreate the sign of the speed
  if (speed_input > 0.0)
    speed_input_squared *= -1.0; 

  twist.linear.x = -0.55 * speed_input_squared; 
  
  twist.angular.z = msg->axis(_axisActions->moveLeftRight);
  sensorHead.pitch = msg->axis(_axisActions->shPitch) * _threshSpeedSensorHead;  
  sensorHead.yaw   = msg->axis(_axisActions->shYaw) * _threshSpeedSensorHead; 
  if(msg->button(_btnActions->shHome).state())
    sensorHead.home = true;
/*
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
  }*/

  // map flippers
  ohm_teleop_msgs::FlipperAngle flippers;
  double vz = 0.0;
  if(msg->button(_btnActions->flipperUp).state()) 
    vz = -1.0;
  else if(msg->button(_btnActions->flipperDown).state()) 
    vz = 1.0;
  if(msg->button(_btnActions->flipperFr).state()) 
    flippers.back_left = _speedFlipperManual * vz;
  if(msg->button(_btnActions->flipperFl).state()) 
    flippers.back_right = _speedFlipperManual * vz;
  if(msg->button(_btnActions->flipperBr).state()) 
    flippers.front_left = -1.0 * _speedFlipperManual * vz;
  if(msg->button(_btnActions->flipperBl).state()) 
    flippers.front_right = -1.0 * _speedFlipperManual * vz;

  auto comm = Communication::getInstance();

  comm->publishToDrive(twist);
  comm->publishToSensorHead(sensorHead);
  comm->publishToFlippers(flippers);

  static ros::Time timerSwitch = ros::Time::now();
  auto             mapCtrl     = MapperController::getInstance();
  if((msg->button(_btnActions->driveSwitch1).state()) && (msg->button(_btnActions->driveSwitch2).state()))
  {
    const double timePressed = (ros::Time::now() - timerSwitch).toSec();
    if((timePressed > _threshSwitchDir) && !_switched)
    {
      std::cout << __PRETTY_FUNCTION__ << "wanna switch" << std::endl;
      mapCtrl->switchMapper(IMapper::RemoteType::DRIVE, true);
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

void MapperDriveReverse::mapImage(void)
{
  auto hud       = Hud::getInstance();
  auto comm      = Communication::getInstance();
  auto imageMain = comm->imageMain();
  if(imageMain)
  {
    hud->setImageMain(imageMain);
    // hud->update();
  }
}

void MapperDriveReverse::init(void)
{
  auto comm = Communication::getInstance();
  comm->setHoming(_homingPitchRev, _homingYawRev);
  _homingIters = 50;
}

} // namespace phros_remote
