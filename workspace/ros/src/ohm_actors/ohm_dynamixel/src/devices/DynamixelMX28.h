/*
 * DynamixelMX28.h
 *
 *  Created on: Mar 20, 2018
 *      Author: phil
 */

#ifndef ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELMX28_H_
#define ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELMX28_H_

#include "DynamixelDevice.h"

#include "DeviceData.h"

namespace ohm_dynamixel
{

class DynamixelMX28: public DynamixelDevice
{
public:
  DynamixelMX28(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub);
  virtual ~DynamixelMX28();
  virtual uint32_t maxAngle(void)const{return MX28MAX;}
  virtual uint32_t minAngle(void)const{return MX28MIN;}
  virtual float    resolution(void)const{return MX28RES;}
  virtual std::string type(void)const{return MX28TYPE;}
};

} /* namespace ohm_dynamixel */

#endif /* ROS_SRC_NEW_DYN_SRC_DEVICES_DYNAMIXELMX28_H_ */
