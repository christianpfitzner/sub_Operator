/*
 * ohm_joy_rough_node.cpp
 * joy node with primary focus on steering through rough terrain
 * (focus on the fine control of the flippers)
 *  Created on: Nov 22, 2017
 *      Author: phil
 */
/* The axis and buttons of a PS3 controller. */

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include "ohm_teleop_msgs/FlipperSpeed.h"
#include "ohm_actors_msgs/SensorHeadJoy.h"
#include <geometry_msgs/Twist.h>
#include <string>

enum Axis_ps3_sixad
 {
   A1_X = 0,         //!< y axis of left analog stick
   A1_Y,             //!< x axis of left analog stick
   L2,             //!< y axis of right analog stick
   A2_Y,             //!< x axis of right analog stick
   A2_X,               //!< L2
   R2,               //!< R2
 };

 /**
  *  @enum Buttons_ps3
  *  Buttons of the ps3 controller
  */
 enum Buttons_ps3_sixad
 {
   B_X = 0,     //!< SELECT
   B_C,             //!< A1
   B_T,             //!< A2
   B_S,          //!< START
   B_L1,             //!< UP
   B_R1,          //!< RIGHT
   B_L2,           //!< DOWN
   B_R2,           //!< LEFT
   B_SELECT,             //!< L2
   B_START,             //!< R2
   B_PS,             //!< L1
   B_A1,             //!< A1
   B_A2,             //!< A2
   B_UP,             //!< UP
   B_DOWN,          //!< RIGHT
   B_LEFT,           //!< DOWN
   B_RIGHT           //!< LEFT
 };


void callBackJoy(const sensor_msgs::Joy& joy);

static ros::Publisher _pubSensorHead;
static ros::Publisher _pubTwist;
static ros::Publisher _pubFlippers;
static double _threshSpeedFlippers   = 0.0;
static double _threshSpeedLinear     = 0.0;
static double _threshSpeedAngular    = 0.0;
static double _threshSpeedSensorHead = 0.0;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "joy_schroedi");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");

  std::string topicJoy;
  std::string topicSensorHead;
  std::string topicTwist;
  std::string topicFlippers;

  prvNh.param<std::string>("topic_joy", topicJoy, "joy");
  prvNh.param<std::string>("topic_twist", topicTwist, "vel/cmd");
  prvNh.param<std::string>("topic_sensor_head", topicSensorHead, "sensorhead_joy");
  prvNh.param<std::string>("topic_flippers", topicFlippers, "flipper/manual");
  prvNh.param<double>("thresh_speed_flippers"   , _threshSpeedFlippers,   3000.0);
  prvNh.param<double>("thresh_speed_linear"     , _threshSpeedLinear,     1.3);
  prvNh.param<double>("thresh_speed_angular"    , _threshSpeedAngular,    0.8);
  prvNh.param<double>("thresh_speed_sensor_head", _threshSpeedSensorHead, 0.6);

  ros::Subscriber subsJoy = nh.subscribe(topicJoy, 1, callBackJoy);

  _pubSensorHead = nh.advertise<ohm_actors_msgs::SensorHeadJoy>(topicSensorHead, 1);
  _pubTwist = nh.advertise<geometry_msgs::Twist>(topicTwist, 1);
  _pubFlippers = nh.advertise<ohm_teleop_msgs::FlipperSpeed>(topicFlippers, 1);
  ros::spin();
}

void callBackJoy(const sensor_msgs::Joy& joy)
{
  static bool initialized = false;
 //map twist
  if((joy.axes[R2] > 0.7 && joy.axes[L2] > 0.7) && !initialized)  //apparently the ps3 pad needs to be initialized...both accelerators pressed
  {
    ROS_INFO("Joy Node: Initializing...");
    usleep(1000 * 1000);
    initialized = true;
    ROS_INFO("Joy Node: Initialized");
  }
  if(!initialized)
  {
    ROS_INFO_THROTTLE(1.0, "Joy Node: Not initialized");
    return;
  }
  geometry_msgs::Twist twist;
  twist.linear.x =  -0.5 * (joy.axes[R2] - joy.axes[L2]) * _threshSpeedLinear;
  twist.angular.z =  joy.axes[A1_X] * _threshSpeedAngular;
//map sensor head
  ohm_actors_msgs::SensorHeadJoy sensorHead;
  sensorHead.pitch = joy.axes[A2_X] * _threshSpeedSensorHead;
  sensorHead.yaw   = joy.axes[A2_Y] * _threshSpeedSensorHead;
  if(joy.buttons[B_A2])
    sensorHead.home = true;
  //map flippers
  ohm_teleop_msgs::FlipperSpeed flippers;

  double vz = 0.0;
  if(joy.buttons[B_UP])
    vz = 1.0;
  else if(joy.buttons[B_DOWN])
    vz = -1.0;
  //read select button
  if(joy.buttons[B_T])
    flippers.front_left = _threshSpeedFlippers * vz;
  if(joy.buttons[B_C])
    flippers.front_right = _threshSpeedFlippers * vz;
  if(joy.buttons[B_S])
    flippers.back_left = _threshSpeedFlippers * vz;
  if(joy.buttons[B_X])
    flippers.back_right = _threshSpeedFlippers * vz;

  _pubTwist.publish(twist);
  _pubSensorHead.publish(sensorHead);
  _pubFlippers.publish(flippers);
}






