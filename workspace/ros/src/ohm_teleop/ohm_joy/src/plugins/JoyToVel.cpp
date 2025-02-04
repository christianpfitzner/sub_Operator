/*
 * JoyToVelPlugin.cpp
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#include "JoyToVel.h"
#include <geometry_msgs/Twist.h>



namespace ohm_joy {

JoyToVel::JoyToVel(ros::NodeHandle* nh)
: Plugin(nh)
{
   std::string vel_topic;
   ros::NodeHandle private_nh("~");

   private_nh.param("vel_topic",        vel_topic,         std::string("vel/cmd"));

   ros::Publisher vel_pub = _nh->advertise<geometry_msgs::Twist>(vel_topic, 1);

   _pub.push_back(vel_pub);
}

JoyToVel::~JoyToVel()
{
   // TODO Auto-generated destructor stub
}


void JoyToVel::publish(void)
{
   geometry_msgs::Twist twist = _mapper->getTwist();
   _pub.front().publish(twist);
}


} /* namespace ohm_joy */
