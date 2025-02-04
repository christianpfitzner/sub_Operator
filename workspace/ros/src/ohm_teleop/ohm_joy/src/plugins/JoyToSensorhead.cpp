/*
 * JoyToSensorhead.cpp
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#include "JoyToSensorhead.h"

namespace ohm_joy {

JoyToSensorhead::JoyToSensorhead(ros::NodeHandle* nh)
:  Plugin(nh)
{
   ros::NodeHandle private_nh("~");

   std::string sensorhead_topic;

   private_nh.param("sensorhead_topic",  sensorhead_topic,      std::string("sensorhead_joy"));

   ros::Publisher sensorhead_pub = _nh->advertise<ohm_actors_msgs::SensorHeadJoy>(sensorhead_topic, 1);

   _pub.push_back(sensorhead_pub);
}

JoyToSensorhead::~JoyToSensorhead()
{
   // TODO Auto-generated destructor stub
}

void JoyToSensorhead::publish(void)
{
   ohm_actors_msgs::SensorHeadJoy sensorHead = _mapper->getSensorHeadJoy();
   _pub.front().publish(sensorHead);
}

} /* namespace ohm_joy */
