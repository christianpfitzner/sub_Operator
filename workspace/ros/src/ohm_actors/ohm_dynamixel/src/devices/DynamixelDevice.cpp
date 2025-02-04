/*
 * DynamixelDevice.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#include "DynamixelDevice.h"

#include <std_msgs/Float32.h>

#include <ros/ros.h>

namespace ohm_dynamixel
{

DynamixelDevice::DynamixelDevice(const uint8_t& devId, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub):
                                _mode(Mode::POS),
                                _desSpeed(0.0),
                                _desPos  (0.0),
                                //_desTicks(0),
                                _devId(devId),
                                //_devSpeed(0),
                                _hysthVelCtrl(0),
                                _errorControlling(0),
                                _speedPosMode(speedPosMode),
                                _jointName(jointName),
                                _pubPos(nh.advertise<std_msgs::Float32>(_jointName + "/pos/present", 1)),
                                _pubVel(nh.advertise<std_msgs::Float32>(_jointName + "/vel/present", 1)),
                                _pubCurrent(nh.advertise<std_msgs::Float32>(_jointName + "/current/present", 1)),
                                _flagsPub(flagsPub),
                                // _speedPosModeAngular(0.0f),
                                _angPresent(0.0f),
                                _spdPresent(0.0f),
                                _curPresent(0.0f),
                                _posLast(0),
                                _hysthReached(false),
                                _posStop(0)
{
  ros::NodeHandle prvNh("~");
  int varHyst = 0;
  int varCtrl = 0;
  prvNh.param<int>("hysth_vel_ctrl", varHyst, 20);   //todo: maybe move these parameters to xml config in case different parameters are needed for different servos
  prvNh.param<int>("error_ctrl", varCtrl, 3);
  _hysthVelCtrl = static_cast<uint32_t>(varHyst);
  _errorControlling = static_cast<uint32_t>(varCtrl);
}

DynamixelDevice::~DynamixelDevice()
{
  // TODO Auto-generated destructor stub
}

void DynamixelDevice::controlSpeed(const uint32_t curPos, uint32_t* const pos, uint32_t* const speed)
{
  *pos = 0;
  *speed = this->speedRadToTicks(_desSpeed);
  //  std::cout << " id " << static_cast<int>(_devId) << " curpos " << curPos << std::endl;
  if(_desSpeed > 0.0)
    *pos = this->maxAngle();
  else
    *pos = this->minAngle();

  if(*speed <= _hysthVelCtrl)  //very low controlling error todo: use PID controller instead
  {
    *speed = 1;
    *pos = _posLast;
    int32_t diff = static_cast<int32_t>(curPos) - static_cast<int32_t>(_posLast);
    //   std::cout << " in hyst : diff = " << std::abs(diff) << " " << static_cast<int32_t>(*pos)  << " - " << static_cast<int32_t>(_posLast) << std::endl;

    if(std::abs(diff) < _errorControlling)
    {
      if(!_hysthReached)
      {
        _hysthReached = true;
        _posStop = curPos;
      }
      *speed = 1;
      *pos = _posStop;
    }
  }
  else
    _hysthReached = false;
  _posLast = curPos;
}

void DynamixelDevice::callBackSpeed(const std_msgs::Float32& msg)
{
  _desSpeed = msg.data;
  _mode = Mode::VEL;
}

void DynamixelDevice::callBackPosition(const std_msgs::Float32& msg)
{
  _desPos = msg.data;
  _mode = Mode::POS;
}

uint32_t DynamixelDevice::speedRadToTicks(const float speedRad)
{
  static const float fac = 1.0 / (this->resolution() * (M_PI / 180.0));
  return static_cast<uint32_t>(std::round(std::abs(speedRad * fac)));
}

void DynamixelDevice::pubPos(const float& pos)
{
  std_msgs::Float32 msg;
  msg.data = pos;
  _pubPos.publish(msg);
}

void DynamixelDevice::pubData(DynamixelWorkbench& dxlWb)
{
  std_msgs::Float32 msg;
  if(_flagsPub.pubPos)
  {
    msg.data = _angPresent;
    _pubPos.publish(msg);
  }
  if(_flagsPub.pubVel)
  {
    msg.data = _spdPresent;
    _pubVel.publish(msg);
  }
  if(_flagsPub.pubCurrent)
  {
    msg.data = _curPresent;
    _pubCurrent.publish(msg);
  }
}

void DynamixelDevice::update(DynamixelWorkbench& dxlWb)
{
  //read necessary data from device
  int32_t posCurrent = 0;
  float angleCurrent = 0.0;
  float velCurrentAngular = 0.0;
  int32_t currentPres = 0;
  //  std::cout << __PRETTY_FUNCTION__ << " sd " << static_cast<int>(_devId) << " " <<  dxlWb.itemRead(_devId, "Shutdown") << std::endl;
  //  int8_t err = static_cast<int8_t>(dxlWb.itemRead(_devId, "Shutdown"));
  //  std::cout << std::endl;
  //  for(unsigned int i = 7; i > 0; i--)
  //  {
  //    std::cout << ((err >> i) & 1);
  //  }
  //  std::cout << std::endl;

  if((_mode == Mode::VEL) || _flagsPub.pubPos)
  {
    dxlWb.itemRead(_devId, "Present_Position", &posCurrent);
    angleCurrent = dxlWb.convertValue2Radian(this->_devId, posCurrent);
    _angPresent = angleCurrent;
  }
  if(_flagsPub.pubVel)
  {
    int32_t velCurrentTicks = 0;
    dxlWb.itemRead(_devId, "Present_Velocity", &velCurrentTicks);
    velCurrentAngular = dxlWb.convertValue2Radian(_devId, velCurrentTicks);
    _spdPresent = velCurrentAngular;
  }
  if(_flagsPub.pubCurrent)
  {
    int32_t var = 0;
    dxlWb.itemRead(_devId, "Present_Load", &var);
    _curPresent = static_cast<float>(var);
  }

  if(_mode == DynamixelDevice::Mode::POS)
  {
    const float angle = _desPos;
    const int32_t goalPosition = dxlWb.convertRadian2Value(_devId, angle);
    //std::cout << __PRETTY_FUNCTION__ << " desired anlge " << angle << " = ticks " << goalPosition << std::endl;
    dxlWb.goalVelocity(_devId, static_cast<int>(_speedPosMode));//  goalSpeed(_devId, _speedPosMode);
    //char** log = NULL;
    if(!dxlWb.goalPosition(_devId, static_cast<int>(goalPosition)))
    {
      std::cout << __PRETTY_FUNCTION__ << "error setting goal position of " << goalPosition << std::endl;
    }
  }
  else if(_mode == DynamixelDevice::Mode::VEL)
  {
    uint32_t posDesired = 0;
    uint32_t speedDesired = 0;
    this->controlSpeed(posCurrent, &posDesired, &speedDesired);
    dxlWb.goalVelocity(_devId, static_cast<int>(speedDesired));
    dxlWb.goalPosition(_devId, static_cast<int>(posDesired));
  }
  else
    ROS_ERROR("%s - error: Unknown mode selected (should not happen)", __PRETTY_FUNCTION__);

  this->pubData(dxlWb);
}

} /* namespace ohm_dynamixel */
