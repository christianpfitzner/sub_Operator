/*
 * joy_to_arm.cpp
 *
 *  Created on: Mar 22, 2019
 *      Author: phil
 */

#include <ros/ros.h>
#include "ohm_teleop_msgs/ArmCommand.h"
#include <sensor_msgs/Joy.h>
#include "Ps3Profiles.h"
#include <string>

void callBackJoy(const sensor_msgs::Joy& joy);

static ros::Publisher _pubArmCommand;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "joy_to_arm");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");

  std::string topicJoy;
  std::string topicArmCommand;

  prvNh.param<std::string>("topic_joy", topicJoy, "joy");
  prvNh.param<std::string>("topic_arm_command", topicArmCommand, "arm_command");

  ros::Subscriber subsJoy = nh.subscribe(topicJoy, 1, callBackJoy);
  _pubArmCommand = nh.advertise<ohm_teleop_msgs::ArmCommand>(topicArmCommand, 1);
  ros::spin();
}

void callBackJoy(const sensor_msgs::Joy& joy)
{
  static bool initial = true;
  static sensor_msgs::Joy last;
  if(initial)
  {
    last = joy;
    initial = false;
    return;
  }
  ohm_teleop_msgs::ArmCommand command;
  //map axes
  command.xe = joy.axes[A1_Y];
  command.ye = joy.axes[A1_X];
  command.ze = joy.axes[A2_Y];
  //map "buttons"
  if(joy.buttons[B_SELECT] && !last.buttons[B_SELECT])
    command.change_coordinate_system = true;
  //  if(joy.buttons[B_C] && !last.buttons[B_C])
  command.grzu = joy.buttons[B_C];
  //if(joy.buttons[B_S] && !last.buttons[B_S])
  command.grauf = joy.buttons[B_S];
  command.pminus = joy.buttons[B_L1];
  command.pplus = joy.buttons[B_R1];
  command.rminus = joy.buttons[B_R2];
  command.rplus  = joy.buttons[B_L2];
  command.initial_pose = joy.buttons[B_START];
  command.yminus = joy.buttons[B_LEFT];
  command.yplus = joy.buttons[B_RIGHT];
  command.initial_up = joy.buttons[B_UP];
  command.initial_look_down = joy.buttons[B_DOWN];
  command.initial_tunnel_side = joy.buttons[B_X];
  command.initial_tunnel_up = joy.buttons[B_T];
  _pubArmCommand.publish(command);
  last = joy;
}

