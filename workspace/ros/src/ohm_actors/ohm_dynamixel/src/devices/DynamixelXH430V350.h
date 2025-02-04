/*
 * DynamixelXH430V350.h
 *
 *  Created on: Apr 23, 2019
 *      Author: phil
 */

#ifndef OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELXH430V350_H_
#define OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELXH430V350_H_

#include "DynamixelDevice.h"
#include "DeviceData.h"

namespace ohm_dynamixel
{

class DynamixelXH430V350 : public DynamixelDevice
{
public:
  DynamixelXH430V350(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub);
  virtual ~DynamixelXH430V350();
  virtual uint32_t maxAngle(void)const{return XH430V350MAX;}
  virtual uint32_t minAngle(void)const{return XH430V350MIN;}
  virtual float  resolution(void)const{return XH430V350RES;}
  virtual std::string  type(void)const{return XH430V350TYPE;}
};

} /* namespace ohm_dynamixel */

#endif /* OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELXH430V350_H_ */
