/*
 * arm_interface.cpp
 *
 *  Created on: Mar 22, 2019
 *      Author: phil
 */


#include <ros/ros.h>
#include "ArmInterFace.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "arm_interface");
  ArmInterFace interface;
  ros::spin();
}


