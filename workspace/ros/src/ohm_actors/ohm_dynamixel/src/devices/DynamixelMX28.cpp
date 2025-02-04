/*
 * DynamixelMX28.cpp
 *
 *  Created on: Mar 20, 2018
 *      Author: phil
 */

#include "DynamixelMX28.h"

namespace ohm_dynamixel
{

DynamixelMX28::DynamixelMX28(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub):
    DynamixelDevice(id, speedPosMode, jointName, nh, flagsPub)
{
  // TODO Auto-generated constructor stub

}

DynamixelMX28::~DynamixelMX28()
{
  // TODO Auto-generated destructor stub
}

} /* namespace ohm_dynamixel */
