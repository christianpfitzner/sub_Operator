/*
 * DynamixelMX106.h
 *
 *  Created on: May 13, 2019
 *      Author: phil
 */

#ifndef OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELMX106_H_
#define OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELMX106_H_

#include "DynamixelDevice.h"
#include "DeviceData.h"

namespace ohm_dynamixel
{

class DynamixelMX106: public DynamixelDevice
{
public:
  DynamixelMX106(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub);
  virtual ~DynamixelMX106();
  virtual uint32_t maxAngle(void)const{return MX106MAX;}
   virtual uint32_t minAngle(void)const{return MX106MIN;}
   virtual float    resolution(void)const{return MX106RES;}
   virtual std::string type(void)const{return MX106TYPE;}
};

} /* namespace ohm_dynamixel */

#endif /* OHM_DYNAMIXEL_SRC_DEVICES_DYNAMIXELMX106_H_ */
