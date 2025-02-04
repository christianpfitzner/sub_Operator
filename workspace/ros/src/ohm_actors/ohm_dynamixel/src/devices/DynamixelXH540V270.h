/*
 * DynamixelXH540V270.h
 *
 *  Created on: May 29, 2019
 *      Author: phil
 */

#ifndef OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELXH540V270_H_
#define OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELXH540V270_H_

#include "DynamixelDevice.h"
#include "DeviceData.h"

class DynamixelXH540V270: public ohm_dynamixel::DynamixelDevice
{
public:
  DynamixelXH540V270(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub);
  virtual ~DynamixelXH540V270();
  virtual uint32_t maxAngle(void)const{return XH540V270MAX;}
  virtual uint32_t minAngle(void)const{return XH540V270MIN;}
  virtual float  resolution(void)const{return XH540V270RES;}
  virtual std::string  type(void)const{return XH540V270TYPE;}
};

#endif /* OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELXH540V270_H_ */
