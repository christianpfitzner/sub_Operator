/*
 * DynamixelXH430V350.cpp
 *
 *  Created on: Apr 23, 2019
 *      Author: phil
 */

#include "DynamixelXH430V350.h"

namespace ohm_dynamixel
{

DynamixelXH430V350::DynamixelXH430V350(const uint8_t& id, const uint32_t& speedPosMode, const std::string& jointName, ros::NodeHandle& nh, const FlagsPub& flagsPub):
     DynamixelDevice(id, speedPosMode, jointName, nh, flagsPub)
{

}

DynamixelXH430V350::~DynamixelXH430V350()
{

}

} /* namespace ohm_dynamixel */
