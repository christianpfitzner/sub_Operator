/*
 * DynamixelAX12A.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#include "DynamixelAX12A.h"

namespace ohm_dynamixel
{

DynamixelAX12A::DynamixelAX12A(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub):
    DynamixelDevice(id, speedPosMode, jointName, nh, flagsPub)
{
  // TODO Auto-generated constructor stub

}

DynamixelAX12A::~DynamixelAX12A()
{
  // TODO Auto-generated destructor stub
}

} /* namespace ohm_dynamixel */
