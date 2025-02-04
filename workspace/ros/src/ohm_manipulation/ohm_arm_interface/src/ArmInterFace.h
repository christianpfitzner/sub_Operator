/*
 * ArmInterFace.h
 *
 *  Created on: Mar 21, 2019
 *      Author: phil
 */

#ifndef OHM_ARM_INTERFACE_SRC_ARMINTERFACE_H_
#define OHM_ARM_INTERFACE_SRC_ARMINTERFACE_H_

#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include "ohm_teleop_msgs/ArmCommand.h"
#include "ohm_teleop_msgs/SetArmPredefinedPos.h"
#include "PredefinedArmPose.h"
#include "ohm_arm_interface/ReconfigureConfig.h"
#include <dynamic_reconfigure/server.h>

class ArmInterFace
{
public:
  ArmInterFace();
  virtual ~ArmInterFace();
private:
  void callBackTimer(const ros::TimerEvent& ev);
  void callBackArmCommand(const ohm_teleop_msgs::ArmCommand& cmd);
  void callBackJointAngles(const sensor_msgs::JointState& jsUpdate);
  void publishToInverse(void);
  void remapCommand(const ohm_teleop_msgs::ArmCommand& joyInput);
  void setNewGoalPosition();
  std::vector<double> getSelfcollisionDetectionPosition();
  bool checkSpaceConstraints();
  void setInitialPosition(void);
  bool callBackServicePreDefinedCmd(ohm_teleop_msgs::SetArmPredefinedPos::Request& req, ohm_teleop_msgs::SetArmPredefinedPos::Response& res);
  void setPredefinedPose(const PredefinedArmPose& pos);
  void callbackDynamicReconfigure(ohm_arm_interface::ReconfigureConfig& config, uint32_t level);
  ros::NodeHandle _nh;
  ros::Publisher  _pubMoveToInverse;
  ros::Publisher  _pubGripperCsActive;
  ros::Publisher  _pubReachedSpaceConstraints;
  ros::Subscriber _subsJointAnglesInverse;
  ros::Subscriber _subsArmCommand;
  sensor_msgs::JointState _actualJointPosition;
  std::vector<double> _relativeRotationChange{0.0, 0.0, 0.0};
  std::vector<double> _relativePositionChange{0.0, 0.0, 0.0};
  std::vector<double> _newRotationMatrix{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  std::vector<double> _selfcollisionMax{0.450, 0.220, 0.05}; //use in get_selcollision_detection_position()
  std::vector<double> _selfcollisionMin{-0.300, -0.220, 0.0}; //use in get_selcollision_detection_position()
  double _maxSpaceConstrain = 1.22;
  double _linearScalePosition{0.0045};
  double _linearScaleRotation{0.05};
  double _linearScaleGripper{0.05};
  bool _reachedSpaceConstrains = false;
  bool _oldCoordinateSystem = false;
  bool _changeCoordinateSystem = false;
  double _coordianteSystem{0.0};
  unsigned int _nrOfCmds   = 9;
  unsigned int _nrOfJoints = 7;
  unsigned int _nrOfPos    = 3;
  bool _signalToMoveArmInReal;
  ros::Timer _timer;
  ros::Time _armCommandLast;
  ohm_teleop_msgs::ArmCommand _comand;
  ros::ServiceServer _serverArmPredefinedCmd;
  std::vector<PredefinedArmPose> _predefinedPoses;
  dynamic_reconfigure::Server<ohm_arm_interface::ReconfigureConfig> _serverReconf; ///< ROS dynamic reconfigure server
  dynamic_reconfigure::Server<ohm_arm_interface::ReconfigureConfig>::CallbackType _callBackConfig; ///< ROS dynamic reconfigure object
};

#endif /* OHM_ARM_INTERFACE_SRC_ARMINTERFACE_H_ */
