/*
 * ohm_joy_advanced_node.cpp
 *
 *  Created on: 14.06.2016
 *      Author: chris
 */


#include <ros/ros.h>

#include "plugins/JoyToVel.h"
#include "plugins/JoyToFlipper.h"
#include "plugins/JoyToManipulator.h"
#include "plugins/JoyToSensorhead.h"


#include <ohm_teleop_msgs/BackwardDriving.h>


ros::Subscriber    g_joy_sub;
ros::ServiceServer g_backward_srv;
JoyMapper          g_mapper;


ohm_joy::JoyToVel*          vel_plugin;
ohm_joy::JoyToFlipper*      flip_plugin;
ohm_joy::JoyToManipulator*  manip_plugin;
ohm_joy::JoyToSensorhead*   sensorhead_plugin;



void joyCallback(const sensor_msgs::Joy& msg)
{
   // config mapper
   g_mapper.setJoyMsg(msg);
   g_mapper.map();

   // forward remaped signal to plugins
   vel_plugin->publish();
   flip_plugin->publish();
//   manip_plugin->publish();
   sensorhead_plugin->publish();

}


bool changeMode(ohm_teleop_msgs::BackwardDriving::Request  &req,
         ohm_teleop_msgs::BackwardDriving::Response &res)
{
   g_mapper.setOrientation(req.driveReverse);
   return true;
}

int main(int argc, char **argv)
{
   ros::init(argc, argv, "joy_advanced_node");
   ros::NodeHandle nh;
   ros::NodeHandle private_nh("~");

   vel_plugin        = new ohm_joy::JoyToVel(&nh);
   flip_plugin       = new ohm_joy::JoyToFlipper(&nh);
   manip_plugin      = new ohm_joy::JoyToManipulator(&nh);
   sensorhead_plugin = new ohm_joy::JoyToSensorhead(&nh);


   std::string joy_topic;
   std::string config_file;
   std::string reverse_mode_service_topic;

   private_nh.param("joy_topic",        joy_topic,                        std::string("joy"));
   private_nh.param("config_file",      config_file,                      std::string("/home/chris/ps3.ini"));
   private_nh.param("reverse_mode",     reverse_mode_service_topic,       std::string("reverse_mode"));


   g_joy_sub         = nh.subscribe(joy_topic,    20, &joyCallback);
   g_backward_srv    = nh.advertiseService(reverse_mode_service_topic, changeMode);

   g_mapper.setConfigFile(config_file);

   vel_plugin->setMapper(&g_mapper);
   flip_plugin->setMapper(&g_mapper);
   manip_plugin->setMapper(&g_mapper);
   sensorhead_plugin->setMapper(&g_mapper);


   while(ros::ok()) {
      ros::spin();
      std::cout << "what" << std::endl;
   }


   delete vel_plugin;
   delete flip_plugin;
   delete manip_plugin;
   delete sensorhead_plugin;

}

