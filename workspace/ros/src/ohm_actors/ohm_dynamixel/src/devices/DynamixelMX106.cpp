/*
 * DynamixelMX106.cpp
 *
 *  Created on: May 13, 2019
 *      Author: phil
 */

#include "DynamixelMX106.h"

namespace ohm_dynamixel
{

DynamixelMX106::DynamixelMX106(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub):
        DynamixelDevice(id, speedPosMode, jointName, nh, flagsPub)
{
  // TODO Auto-generated constructor stub

}

DynamixelMX106::~DynamixelMX106()
{
  // TODO Auto-generated destructor stub
}

} /* namespace ohm_dynamixel */
