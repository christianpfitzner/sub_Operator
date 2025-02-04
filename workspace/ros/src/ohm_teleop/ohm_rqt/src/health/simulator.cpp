/*
 * simulator.cpp
 *
 *  Created on: Mar 15, 2018
 *      Author: juicy
 */


#include <ros/ros.h>
#include "ohm_rqt/pu.h"

int main (int argc, char** argv)
{

ros::init(argc, argv, "pupublisher");

ros::NodeHandle n;

ros::Publisher pu_chatter = n.advertise<ohm_rqt::pu>("puSimulator", 1);

ros::Rate loop_rate(2);

int count = 0;
while(ros::ok())
{
  ++count;
	ohm_rqt::pu puOne;

	for (double i = 48; i >= 0; i -= 0.5)
	{
    puOne.r_5V = i/10;
    puOne.c_5V = i/10;

    puOne.r_12V = i/4;
    puOne.c_12V = i/4;

    puOne.r_24V = i/4;
    puOne.c_24V = i/4;

    puOne.r_48V = i;
    puOne.c_48V = i;

    if(count %2 == 0)
      puOne.alarm_24V = true;


    pu_chatter.publish(puOne);
    loop_rate.sleep();
	}

	loop_rate.sleep();

}

return 0;

}
