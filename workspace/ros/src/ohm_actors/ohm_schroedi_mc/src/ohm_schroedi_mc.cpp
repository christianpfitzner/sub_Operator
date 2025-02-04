/*
 * ohm_schroedi_mc.cpp
 *
 *  Created on: Apr 25, 2017
 *      Author: phil
 */

#include <ros/ros.h>

#include "SchroediMC.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "ohm_schroedi_mc");
  SchroediMC motorcontroller;
  motorcontroller.start();
}



