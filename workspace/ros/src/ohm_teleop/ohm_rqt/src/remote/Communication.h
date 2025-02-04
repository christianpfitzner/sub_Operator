/*
 * Communication.h
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_COMMUNICATION_H_
#define OHM_RQT_SRC_REMOTE_COMMUNICATION_H_

#include <memory>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Bool.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include "utils/SuperSubscriber.h"
#include "utils/SuperImageSubscriber.h"
#include "ohm_schroedi_mc/FlipperPreset.h"
#include "ohm_power_unit/pu.h"
#include "ohm_schroedi_mc/mc_ret.h"
#include "ohm_teleop_msgs/FlipperAngle.h"
#include "ohm_actors_msgs/SensorHeadJoy.h"

namespace ohm_remote
{

class Communication
{
public:
  enum class Cams
  {
    DRIVER = 0,
    GRIPPER
  };
  static std::shared_ptr<Communication> getInstance(void);
  virtual ~Communication();
  std::shared_ptr<sensor_msgs::Joy> joyData(void){return _subsJoy.data();}
  std::shared_ptr<sensor_msgs::Image> imageMain(void){return _subsImageMain.data();}
  std::shared_ptr<sensor_msgs::Image> imageGripper(void){return _subsImageGripper.data();}
  std::shared_ptr<ohm_schroedi_mc::mc_ret> mcRet(void){return _subsMcRet.data();}
  std::shared_ptr<std_msgs::Bool> csActive(void){return _subsGripperCs.data();}
  std::shared_ptr<std_msgs::Bool> armConstraints(void){return _subsArmConstraints.data();}
  const std::shared_ptr<ohm_power_unit::pu> power(void)const{return _subsPu.data();}
  bool sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset);
  void startStopCamera(const Cams& cam, const bool startStop);
  bool republishImages(void);
  bool setHoming(const double homingPitch, const double homingYaw);
  void publishToDrive(const geometry_msgs::Twist& vel){_pubToDrives.publish(vel);}
  void publishToSensorHead(const ohm_actors_msgs::SensorHeadJoy& sens){_pubSensorHead.publish(sens);}
  void publishToFlippers(const ohm_teleop_msgs::FlipperAngle& flippers){_pubFlippers.publish(flippers);}
private:
  void callBackJoy(const sensor_msgs::Joy& joy);
  Communication();
  static std::shared_ptr<Communication> _instance;
  //ros::Subscriber _subsJoy;
  SuperSubscriber<sensor_msgs::Joy> _subsJoy;
  SuperSubscriber<ohm_power_unit::pu> _subsPu;
  SuperSubscriber<ohm_schroedi_mc::mc_ret> _subsMcRet;
  SuperSubscriber<std_msgs::Bool> _subsGripperCs;
  SuperSubscriber<std_msgs::Bool> _subsArmConstraints;
  SuperImageSubscriber _subsImageMain;
  SuperImageSubscriber _subsImageGripper;
  ros::ServiceClient _clientFlipperPreset;
  ros::ServiceClient _clientSetHoming;
  std::unique_ptr<image_transport::ImageTransport> _it;
  image_transport::Publisher _pubReImageMain;
  image_transport::Publisher _pubReImageGripper;
  ros::Publisher _pubToDrives;
  ros::Publisher _pubFlippers;
  ros::Publisher _pubSensorHead;
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_COMMUNICATION_H_ */
