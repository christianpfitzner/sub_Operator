/*
 * epos_skid_steer_node.cpp
 *
 *  Created on: Mar 24, 2017
 *      Author: phil
 */

#include <ros/ros.h>

#include "EposSkidSteerNode.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "epos_skid_steer_node");
  autonohm::EposSkidSteerNode volksbot;
  volksbot.start();
}



