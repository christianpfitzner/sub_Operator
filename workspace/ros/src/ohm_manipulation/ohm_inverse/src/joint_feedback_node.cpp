/*opic
 * joint_feedback_node.cpp
 *
 *  Created on: Oct 16, 2018
 *      Author: phil
 */

#include <ros/ros.h>
#include <string>

#include "ohm_robotarm/DriveState.h"
#include <sensor_msgs/JointState.h>
#include <std_msgs/Float32.h>

void callBackJoint1(const ohm_robotarm::DriveState& state);
void callBackJoint2(const ohm_robotarm::DriveState& state);
void callBackJoint3(const ohm_robotarm::DriveState& state);
void callBackJoint4(const ohm_robotarm::DriveState& state);
void callBackJoint5(const std_msgs::Float32& state);
void callBackJoint6(const std_msgs::Float32& state);
void callBackJoint7(const std_msgs::Float32& state);
void callBackJointGripper1(const std_msgs::Float32& state);
void callBackJointGripper2(const std_msgs::Float32& state);
void timerCallBack(const ros::TimerEvent& ev);

static ros::Publisher _pubJointStates;
static sensor_msgs::JointState _jointStates;

enum class Joints
{
  JOINT_1 = 0,
      JOINT_2,
      JOINT_3,
      JOINT_4,
      JOINT_5,
      JOINT_6,
      JOINT_7,
      GRIPPER_1,
      GRIPPER_2
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "joint_feedback_node");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");

  std::string topicStateJoint1;
  std::string topicStateJoint2;
  std::string topicStateJoint3;
  std::string topicStateJoint4;
  std::string topicStateJoint5;
  std::string topicStateJoint6;
  std::string topicStateJoint7;

  std::string topicStateGripper1;
  std::string topicStateGripper2;

  std::string topicJointStates;

  prvNh.param<std::string>("topic_state_joint1", topicStateJoint1, "/joint_1/state");
  prvNh.param<std::string>("topic_state_joint2", topicStateJoint2, "/joint_2/state");
  prvNh.param<std::string>("topic_state_joint3", topicStateJoint3, "/joint_3/state");
  prvNh.param<std::string>("topic_state_joint4", topicStateJoint4, "/joint_4/state");
  prvNh.param<std::string>("topic_state_joint5", topicStateJoint5, "/joint5/pos/present");
  prvNh.param<std::string>("topic_state_joint6", topicStateJoint6, "/joint6/pos/present");
  prvNh.param<std::string>("topic_state_joint7", topicStateJoint7, "/joint7/pos/present");
  prvNh.param<std::string>("topic_stategripper1", topicStateGripper1, "/gripper1/pos/present");
  prvNh.param<std::string>("topic_stategripper2", topicStateGripper2, "/gripper2/pos/present");
  prvNh.param<std::string>("topic_joint_states", topicJointStates, "joints_real");

  ros::Subscriber subsStateJoint1 = nh.subscribe(topicStateJoint1, 1, callBackJoint1);
  ros::Subscriber subsStateJoint2 = nh.subscribe(topicStateJoint2, 1, callBackJoint2);
  ros::Subscriber subsStateJoint3 = nh.subscribe(topicStateJoint3, 1, callBackJoint3);
  ros::Subscriber subsStateJoint4 = nh.subscribe(topicStateJoint4, 1, callBackJoint4);
  ros::Subscriber subsStateJoint5 = nh.subscribe(topicStateJoint5, 1, callBackJoint5);
  ros::Subscriber subsStateJoint6 = nh.subscribe(topicStateJoint6, 1, callBackJoint6);
  ros::Subscriber subsStateJoint7 = nh.subscribe(topicStateJoint7, 1, callBackJoint7);
  ros::Subscriber subsStateGripper1 = nh.subscribe(topicStateGripper1, 1, callBackJointGripper1);
  ros::Subscriber subsStateGripper2 = nh.subscribe(topicStateGripper2, 1, callBackJointGripper2);

  ros::Timer timerMain = nh.createTimer(ros::Duration(0.1), timerCallBack);

  _pubJointStates = nh.advertise<sensor_msgs::JointState>(topicJointStates, 1);

  _jointStates.name.resize(9);
    _jointStates.position.resize(9, 0.0);

  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_1)] = "joint_1";
  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_2)] = "joint_2";
  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_3)] = "joint_3";
  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_4)] = "joint_4";
  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_5)] = "joint_5";
  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_6)] = "joint_6";
  _jointStates.name[static_cast<unsigned int>(Joints::JOINT_7)] = "joint_7";
  _jointStates.name[static_cast<unsigned int>(Joints::GRIPPER_1)] = "joint_gripper_1";
  _jointStates.name[static_cast<unsigned int>(Joints::GRIPPER_2)] = "joint_gripper_2";

  ros::spin();

}

void callBackJoint1(const ohm_robotarm::DriveState& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_1)] = state.position;
}
void callBackJoint2(const ohm_robotarm::DriveState& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_2)] = -state.position;
}
void callBackJoint3(const ohm_robotarm::DriveState& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_3)] = -(M_PI - state.position);
}
void callBackJoint4(const ohm_robotarm::DriveState& state)
{
//  double val = state.position;
//  j4.position = -M_PI + q4;
//    if (j4.position < 0.0) {
//      j4.position = M_PI + q4;
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_4)] = state.position;
}
void callBackJoint5(const std_msgs::Float32& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_5)] = state.data;
}
void callBackJoint6(const std_msgs::Float32& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_6)] = state.data;
}
void callBackJoint7(const std_msgs::Float32& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::JOINT_7)] = state.data;
}
void callBackJointGripper1(const std_msgs::Float32& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::GRIPPER_1)] = state.data;
}
void callBackJointGripper2(const std_msgs::Float32& state)
{
  _jointStates.position[static_cast<unsigned int>(Joints::GRIPPER_2)] = state.data;
}
void timerCallBack(const ros::TimerEvent& ev)
{
  _jointStates.header.seq++;
  _jointStates.header.stamp = ros::Time::now();
  _pubJointStates.publish(_jointStates);
}
