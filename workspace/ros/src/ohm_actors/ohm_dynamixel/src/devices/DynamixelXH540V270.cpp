/*
 * DynamixelXH540V270.cpp
 *
 *  Created on: May 29, 2019
 *      Author: phil
 */

#include "DynamixelXH540V270.h"

DynamixelXH540V270::DynamixelXH540V270(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub):
DynamixelDevice(id, speedPosMode, jointName, nh, flagsPub)
{
  // TODO Auto-generated constructor stub

}

DynamixelXH540V270::~DynamixelXH540V270()
{
  // TODO Auto-generated destructor stub
}

