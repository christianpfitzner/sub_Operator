/*
 * Communication.cpp
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#include "Communication.h"
#include <iostream>
#include "ohm_actors_msgs/SetHomingSensHead.h"

namespace ohm_remote
{

std::shared_ptr<Communication> Communication::_instance(nullptr);

Communication::Communication():
        _subsJoy("joy", ros::Duration(1.0)),
        _subsImageMain("usb_cam/image_raw", ros::Duration(1.0)),
        _subsImageGripper("gripper/color/image_raw", ros::Duration(1.0)),
        _subsPu("pu", ros::Duration(1.0)),
        _subsMcRet("/mc_ret", ros::Duration(1.0)),
        _subsGripperCs("/inverse/gripper_cs_active", ros::Duration(1.0)),
        _subsArmConstraints("/inverse/arm_reached_space_constraints", ros::Duration(1.0))
{
  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");
  std::string topicTwist;
  std::string topicFlippers;
  std::string topicSensorHead;
  prvNh.param<std::string>("topic_twist", topicTwist, "vel/cmd");
   prvNh.param<std::string>("topic_flippers", topicFlippers, "flipper/manual");
   prvNh.param<std::string>("topic_sensor_head", topicSensorHead, "sensorhead_joy");


  _it = std::unique_ptr<image_transport::ImageTransport>(new image_transport::ImageTransport(nh)); //_subsJoy = nh.subscribe("joy", 1, &Commun //_subsJoy = nh.subscribe("joy", 1, &Communication::callBackJoy, this); //_subsJoy = nh.subscribe("joy", 1, &Communication::callBackJoy, this);ication::callBackJoy, this);
  _pubReImageGripper = _it->advertise("hud/image_gripper", 1);//, image_transport::TransportHints("compressed"));
  _pubReImageMain = _it->advertise("hud/image_main", 1);//, image_transport::TransportHints("compressed"));
  _pubToDrives = nh.advertise<geometry_msgs::Twist>("vel/cmd", 1);
  _pubFlippers   = nh.advertise<ohm_teleop_msgs::FlipperAngle>(topicFlippers, 1);
  _pubSensorHead = nh.advertise<ohm_actors_msgs::SensorHeadJoy>(topicSensorHead, 1);
  _clientFlipperPreset = nh.serviceClient<ohm_schroedi_mc::FlipperPreset>("/flipper/preset");
  std::string topicSetHoming;
  prvNh.param<std::string>("topic_set_homing", topicSetHoming, "set_homing");
  if(ros::service::waitForService(topicSetHoming, ros::Duration(0.5)))
    _clientSetHoming = nh.serviceClient<ohm_actors_msgs::SetHomingSensHead>(topicSetHoming);
  else
    ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " warning. Service " << topicSetHoming << " not reachable");
  std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
}

Communication::~Communication()
{
  _subsJoy.stop();
 _subsArmConstraints.stop();
 _subsPu.stop();
 _subsMcRet.stop();
 _subsGripperCs.stop();
  //_subsJoy.shutdown();
}

std::shared_ptr<Communication> Communication::getInstance(void)
//Communication* Communication::getInstance(void)
{
  if(!_instance)
  {
    //std::cout << __PRETTY_FUNCTION__ << "new instance" << std::endl;
    _instance = std::shared_ptr<Communication>(new Communication);
  }
  //std::shared_ptr<Communication> ptr(_instance.get());
  return _instance;
}

void Communication::callBackJoy(const sensor_msgs::Joy& joy)
{
  //ROS_INFO_STREAM(__PRETTY_FUNCTION__ << " entry");
}

bool Communication::sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset)
{
  ohm_schroedi_mc::FlipperPreset pre;
  pre.request = preset.request;
  return _clientFlipperPreset.call(pre);
}

void Communication::startStopCamera(const Cams& cam, const bool startStop)
{

  switch(cam)
  {
  case Cams::DRIVER:
  {
    if(startStop)
      _subsImageMain.start();
    else
      _subsImageMain.stop();
    break;
  }
  case Cams::GRIPPER:
  {
    if(startStop)
      _subsImageGripper.start();
    else
      _subsImageMain.stop();
    break;
  }
  default:
    break;
  }
}

bool Communication::republishImages(void)
{
  bool somesing = false;
  if(_pubReImageGripper.getNumSubscribers())
  {
    if(this->imageGripper())
    {
      _pubReImageGripper.publish(*this->imageGripper().get());
      somesing = true;
    }
  }
  if(_pubReImageMain.getNumSubscribers())
  {
    if(this->imageMain())
    {
      _pubReImageMain.publish(*this->imageMain().get());
      somesing = true;
    }
  }
  return somesing;
}

bool Communication::setHoming(const double homingPitch, const double homingYaw)
{
  ohm_actors_msgs::SetHomingSensHead setHomin;
  setHomin.request.homingPitch = homingPitch;
  setHomin.request.homingYaw = homingYaw;
  return _clientSetHoming.call(setHomin);
}

//void Communication::publishToDrive(const geometry_msgs::Twist& vel)
//{
//  _pubToDrives.publish(vel);
//}

} /* namespace ohm_remote */
