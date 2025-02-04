/*
 * remote_node.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */


#include <ros/ros.h>
#include "RemoteNode.h"
#include <QApplication>
//#include <sensor_msgs/Joy.h>
//#include <std_msgs/Bool.h>
//
//#include "ohm_teleop_msgs/RemoteProfile.h"
//#include "ohm_teleop_msgs/SetRemoteProfile.h"
//#include "mappers/IMapper.h"
//#include "mappers/MapperDrive.h"
//#include "mappers/MapperArm.h"
//#include <memory>
//#include <string>
//#include "mappers/Ps3Profiles.h"
//
//bool callBackSwitchRemoteProfile(ohm_teleop_msgs::SetRemoteProfile::Request& req, ohm_teleop_msgs::SetRemoteProfile::Response& res);
//void callBackJoy(const sensor_msgs::Joy& joy);
//void callBackTimerMain(const ros::TimerEvent& ev);
//
////static std::unique_ptr<IMapper> _mapper(nullptr);
//static IMapper* _mapper = NULL;
//static ros::NodeHandle* _nh = NULL;
//static ros::Publisher _pubActiveProfile;
//static ros::Publisher _pubMenuOverrideActive;
//static ros::ServiceClient _clientHeavyMetal;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "remote_node");
  std::cout << __PRETTY_FUNCTION__ << " lalala" << std::endl;
  QApplication app(argc, argv);
  std::cout << __PRETTY_FUNCTION__ << " bloeld" << std::endl;
  RemoteNode remote;
//  remote.start();
  std::cout << __PRETTY_FUNCTION__ << " here?" << std::endl;
  return app.exec();
}
//  _nh = new ros::NodeHandle;
//  ros::NodeHandle prvNh("~");
//
//  std::string topicJoy;
//  std::string topicSwitchRemote;
//  std::string topicActiveRemote;
//  std::string topicMenuOverride;
//  double loopRate = 0.0;
//
//  prvNh.param<std::string>("topic_joy", topicJoy, "joy");
//  prvNh.param<std::string>("topic_switch_remote", topicSwitchRemote, "switch_remote");
//  prvNh.param<std::string>("topic_active_profile", topicActiveRemote, "active_profile");
//  prvNh.param<std::string>("topic_menu_override", topicMenuOverride, "remote/menu_override");
//  prvNh.param<double>("loop_rate", loopRate, 20.0);
//
//  //_mapper = new MapperDrive(*_nh);
//  _mapper = new MapperArm(*_nh);
//
//  ros::Subscriber subsJoy = _nh->subscribe(topicJoy, 1, callBackJoy);
//  ros::ServiceServer srvSwitchRemote = _nh->advertiseService(topicSwitchRemote, callBackSwitchRemoteProfile);
//  ros::Timer timerMain = _nh->createTimer(ros::Duration(1.0 / loopRate), callBackTimerMain);
//  _pubActiveProfile = _nh->advertise<ohm_teleop_msgs::RemoteProfile>(topicActiveRemote, 1);
//  _pubMenuOverrideActive = _nh->advertise<std_msgs::Bool>(topicMenuOverride, 1);
//
//  ros::spin();
//
//}
//
//bool callBackSwitchRemoteProfile(ohm_teleop_msgs::SetRemoteProfile::Request& req, ohm_teleop_msgs::SetRemoteProfile::Response& res)
//{
//  if((req.remote.remote == ohm_teleop_msgs::RemoteProfile::MAP_DRIVE) && (_mapper->type() == IMapper::RemoteType::DRIVE))
//    return false;
//  else if((req.remote.remote == ohm_teleop_msgs::RemoteProfile::MAP_ARM) && (_mapper->type() == IMapper::RemoteType::ARM))
//    return false;
//  else if(req.remote.remote == ohm_teleop_msgs::RemoteProfile::MAP_ARM)
//    _mapper = new MapperArm(*_nh);
//  else if(req.remote.remote == ohm_teleop_msgs::RemoteProfile::MAP_DRIVE)
//    _mapper = new MapperDrive(*_nh);
//  return true;
//}
//
//void callBackJoy(const sensor_msgs::Joy& joy)
//{
//  static bool initialized = false;
//  static bool psLast = true;
//  static bool menuOverride = false;
//  //map twist
//  if((joy.axes[R2] > 0.7 && joy.axes[L2] > 0.7) && !initialized)  //apparently the ps3 pad needs to be initialized...both accelerators pressed
//  {
//    ROS_INFO("Joy Node: Initializing...");
//    usleep(1000 * 1000);
//    initialized = true;
//    ROS_INFO("Joy Node: Initialized");
//  }
//  if(!initialized)
//  {
//    ROS_INFO_THROTTLE(1.0, "Joy Node: Not initialized");
//    return;
//  }
//  if(joy.buttons[B_PS] && !psLast)
//    menuOverride = !menuOverride;
//
//  std_msgs::Bool overrideActive;
//  if(!menuOverride)
//  {
//    overrideActive.data = false;
//    _mapper->map(joy);
//  }
//  else
//  {
//    overrideActive.data = true;
//    ROS_INFO_THROTTLE(1.0, "Menu override active");
//  }
//  _pubMenuOverrideActive.publish(overrideActive);
//  psLast = joy.buttons[B_PS];
//}
//
//void callBackTimerMain(const ros::TimerEvent& ev)
//{
//  ohm_teleop_msgs::RemoteProfile profile;
//  if(_mapper->type() == IMapper::RemoteType::DRIVE)
//    profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_DRIVE;
//  else if(_mapper->type() == IMapper::RemoteType::ARM)
//    profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_ARM;
//  else
//    ROS_ERROR("%s error. Unknown profile (should not happen)", __PRETTY_FUNCTION__);
//  _pubActiveProfile.publish(profile);
//}
