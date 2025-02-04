/*
 * PredefinedArmPose.h
 *
 *  Created on: Mar 25, 2019
 *      Author: phil
 */

#ifndef OHM_ARM_INTERFACE_SRC_PREDEFINEDARMPOSE_H_
#define OHM_ARM_INTERFACE_SRC_PREDEFINEDARMPOSE_H_

#include <vector>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include "ohm_teleop_msgs/SetArmPredefinedPosRequest.h"

class PredefinedArmPose
{
public:
  PredefinedArmPose(const uint8_t& type);
  virtual ~PredefinedArmPose();
  uint8_t type(void)const{return _type;}
  const std::vector<double>& relPositionChange(void)const{return _relativePositionChange;}
  const std::vector<double>& rotationMatrix(void)const{return _rotationMatrix;}
  const sensor_msgs::JointState& jointPosition(void)const{return _jointPosition;}
private:
  ros::NodeHandle _prvNh;
  uint8_t _type;
  std::vector<double> _relativePositionChange{NAN, NAN, NAN};
   std::vector<double> _rotationMatrix{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
   sensor_msgs::JointState _jointPosition;
   const std::vector<std::string> _prefixes{"CMD_ZERO_", "CMD_INIT_", "CMD_DOWN_", "CMD_UP_", "CMD_SIDE_RIGHT_" , "CMD_SIDE_LEFT_"};
};

#endif /* OHM_ARM_INTERFACE_SRC_PREDEFINEDARMPOSE_H_ */
