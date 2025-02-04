/*
 * main.cpp
 *
 *  Created on: Jan 16, 2014
 *      Author: user
 */


#include <ros/ros.h>
#include "SummitXlBridgeNode.h"

int main(int argc, char **argv)
{
	ros::init(argc, argv,"ohm_summitxl_bridge_node");

	SummitXlBridgeNode s;
	s.start();
}
