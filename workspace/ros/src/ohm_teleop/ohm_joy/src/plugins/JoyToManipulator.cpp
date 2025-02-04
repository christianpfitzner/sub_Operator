/*
 * JoyToManipulator.cpp
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#include "../plugins/JoyToManipulator.h"

namespace ohm_joy {


enum ManiMode{
         VEL,
         POS
};

JoyToManipulator::JoyToManipulator(ros::NodeHandle* nh)
: Plugin(nh)
{
   ros::NodeHandle private_nh("~");

   std::string mani_pos_topic;
   std::string mani_vel_topic;

   private_nh.param("mani_vel_topic",  mani_vel_topic,      std::string("mani_vel_topic"));
   private_nh.param("mani_pos_topic",  mani_pos_topic,      std::string("mani_pos_topic"));

   ros::Publisher vel_pub; // = _nh->advertise<ohm_teleop_msgs::FlipperSpeed>(flipper_vel_topic, 1);
   ros::Publisher pos_pub; // = _nh->advertise<ohm_teleop_msgs::FlipperAngle>(flipper_pos_topic, 1);

   _pub.push_back(vel_pub);
   _pub.push_back(pos_pub);
}

JoyToManipulator::~JoyToManipulator()
{
   // TODO Auto-generated destructor stub
}


void JoyToManipulator::publish(void)
{
   std::cout << "not implemented" << std::endl;
}


} /* namespace ohm_joy */
