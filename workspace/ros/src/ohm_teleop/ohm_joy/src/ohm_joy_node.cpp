
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>

#include <ohm_teleop_msgs/Action.h>
#include <ohm_teleop_msgs/FlipperAngle.h>
#include <ohm_actors_msgs/Command.h>

#include "JoyMapper.h"

#include <string>

// subscribers
ros::Subscriber   _joy_sub;
ros::Subscriber   _state_sub;

// publishers
ros::Publisher    _vel_pub;
ros::Publisher    _action_pub;
ros::Publisher    _flipper_pub;
ros::Publisher    _sensorhead_pub;
ros::Publisher    _inspector_pub;

// variables for state machine
std_msgs::String  _state;

JoyMapper _mapper;

void joyCallback(const sensor_msgs::Joy& msg)
{
   bool block_velocity = false;

   if(_state.data == "manipulator")
   {
      _mapper.setMaxVelocityThrottle(0.2);
      if(_mapper.getTeleopAction().status == _mapper.getTeleopAction().SWITCH_TELE_ASS) block_velocity = false;
      else                                                                              block_velocity = true;
   }
   else {
      _mapper.setMaxVelocityThrottle(0.8);
      block_velocity = false;
   }


   // config mapper
   _mapper.setJoyMsg(msg);
   _mapper.map();

   // publish messages
   if(!block_velocity)
   {
      _vel_pub.publish(       _mapper.getTwist());
      _sensorhead_pub.publish(_mapper.getSensorHeadJoy());
      _flipper_pub.publish(_mapper.getFlipperSpeed());
   }
   else
   {
      _inspector_pub.publish( _mapper.getInspector());
   }

   if(_mapper.getInspector().moveHome == true || _mapper.getInspector().movePark == true)
      _inspector_pub.publish(_mapper.getInspector());

   _action_pub.publish(    _mapper.getTeleopAction());
}


void stateCallback(const std_msgs::String& state)
{
   _state.data = state.data;
}

int main(int argc, char **argv)
{
   ros::init(argc, argv, "joy_config_node");
   ros::NodeHandle _nh;
   ros::NodeHandle private_nh("~");

   // set up topic names
   std::string joy_topic;
   std::string vel_topic;
   std::string flipper_topic;
   std::string action_topic;
   std::string config_file;
   std::string sensorhead_topic;
   std::string inspector_topic;
   std::string simon_state_topic;
   double vMax = 0.0;

   // parametrize subscribers / publishers
   private_nh.param("joy_topic",        joy_topic,         std::string("joy"));
   private_nh.param("vel_topic",        vel_topic,         std::string("vel_cmd"));
   private_nh.param("flipper_topic",    flipper_topic,     std::string("flipper_cmd"));
   private_nh.param("action_topic",     action_topic,      std::string("joy_action"));
   private_nh.param("config_file",      config_file,       std::string("default.ini"));
   private_nh.param("sensorhead_topic", sensorhead_topic,  std::string("sensorhead_joy"));
   private_nh.param("inspector_topic",  inspector_topic,   std::string("inspector_cmd"));
   private_nh.param("simon_state",      simon_state_topic, std::string("cortex/state"));
   private_nh.param<double>("v_max", vMax, 1.2);



   // subscribers
   _joy_sub          = _nh.subscribe(                      joy_topic,    20, &joyCallback);
   _state_sub        = _nh.subscribe(simon_state_topic,                  20, &stateCallback);

   // publishers
   _vel_pub          = _nh.advertise<geometry_msgs::Twist>(vel_topic,        1);
   _flipper_pub      = _nh.advertise<ohm_teleop_msgs::FlipperSpeed>(flipper_topic,     1);
   _action_pub       = _nh.advertise<ohm_teleop_msgs::Action>( action_topic,           1);
   _sensorhead_pub   = _nh.advertise<ohm_actors_msgs::SensorHeadJoy>(sensorhead_topic, 1);
   _inspector_pub    = _nh.advertise<ohm_actors_msgs::Command>(inspector_topic,        1);

   _mapper.setConfigFile(config_file);
   _mapper.setVMax(vMax);
   while(ros::ok()) {
      ros::spin();
      std::cout << "what" << std::endl;
   }

   return 0;
}
