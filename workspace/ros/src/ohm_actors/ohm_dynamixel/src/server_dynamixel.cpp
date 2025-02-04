/*
 * server_dynamixel.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#include <ros/ros.h>

#include "ServerDynamixel.h"

#include <iostream>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "dynamixel_server");
  ros::NodeHandle nh;
  ohm_dynamixel::ServerDynamixel server;

  ros::spin();
}



