/*
 * JoyToFlipper.cpp
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */

#include "../plugins/JoyToFlipper.h"

#include "ohm_teleop_msgs/FlipperAngle.h"
#include "ohm_teleop_msgs/FlipperSpeed.h"

namespace ohm_joy {

enum FlipperMode{
         VEL,
         POS
};

JoyToFlipper::JoyToFlipper(ros::NodeHandle* nh)
: Plugin(nh)
{
   ros::NodeHandle private_nh("~");

   std::string flipper_vel_topic;
   std::string flipper_pos_topic;

   private_nh.param("flipper_vel_topic",        flipper_vel_topic,         std::string("flip/cmd"));
   private_nh.param("flipper_pos_topic",        flipper_pos_topic,         std::string("flipper_cmd_pos"));

   ros::Publisher vel_pub = _nh->advertise<ohm_teleop_msgs::FlipperSpeed>(flipper_vel_topic, 1);
   ros::Publisher pos_pub = _nh->advertise<ohm_teleop_msgs::FlipperAngle>(flipper_pos_topic, 1);

   _pub.push_back(vel_pub);
   _pub.push_back(pos_pub);

}

JoyToFlipper::~JoyToFlipper(void)
{
}

void JoyToFlipper::publish(void)
{
   ohm_teleop_msgs::FlipperSpeed vel = _mapper->getFlipperSpeed();
   _pub[VEL].publish(vel);

//   ohm_teleop_msgs::FlipperAngle pos;
//   pos.front_left = 0.0;
//   pos.front_right= 0.0;
//   pos.back_left  = 0.0;
//   pos.back_right = 0.0;
//   _pub[POS].publish(pos);
}

} /* namespace ohm_joy */
