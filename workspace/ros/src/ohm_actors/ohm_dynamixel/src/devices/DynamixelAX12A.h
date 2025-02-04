/*
 * DynamixelAX12A.h
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#ifndef ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELAX12A_H_
#define ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELAX12A_H_

#include "DynamixelDevice.h"
#include "DeviceData.h"

#include <ros/ros.h>

namespace ohm_dynamixel
{

class DynamixelAX12A: public DynamixelDevice
{
public:
  DynamixelAX12A(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub);
  virtual ~DynamixelAX12A();
  virtual uint32_t maxAngle(void)const{return AX12AMAX;}
  virtual uint32_t minAngle(void)const{return AX12AMIN;}
  virtual float    resolution(void)const{return AX12ARES;}
  virtual std::string type(void)const{return AX12ATYPE;}
};

} /* namespace ohm_dynamixel */

#endif /* ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELAX12A_H_ */
