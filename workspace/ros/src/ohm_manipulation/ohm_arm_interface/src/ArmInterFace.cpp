/*
 * ArmInterFace.cpp
 *
 *  Created on: Mar 21, 2019
 *      Author: phil
 */

#include "ArmInterFace.h"
#include "ohm_arm_interface/RobotarmCtrl.h"
#include "Aktuelle_Position/Aktuelle_Position.h"
#include <std_msgs/Bool.h>
#include "PredefinedArmPose.h"

#include <string>
#include <cmath>

ArmInterFace::ArmInterFace()
{
  _actualJointPosition.name.push_back("joint_1");
  _actualJointPosition.position.push_back(0.0);
  _actualJointPosition.name.push_back("joint_2");
  _actualJointPosition.position.push_back(0.0);
  _actualJointPosition.name.push_back("joint_3");
  _actualJointPosition.position.push_back(M_PI);
  _actualJointPosition.name.push_back("joint_4");
  _actualJointPosition.position.push_back(-M_PI);
  _actualJointPosition.name.push_back("joint_5");
  _actualJointPosition.position.push_back(0.0);
  _actualJointPosition.name.push_back("joint_6");
  _actualJointPosition.position.push_back(0.0);
  _actualJointPosition.name.push_back("joint_7");
  _actualJointPosition.position.push_back(0.0);
  _actualJointPosition.name.push_back("gr1");
  _actualJointPosition.position.push_back(0.0);
  _actualJointPosition.name.push_back("gr2");
  _actualJointPosition.position.push_back(0.0);

  ros::NodeHandle prvNh("~");

  std::string topicMoveToInverse;
  std::string topicGripperCsActive;
  std::string topicArmReachedSpaceContraints;
  std::string topicJointAngleInverse;
  std::string topicArmCommand;
  std::string topicPredefinedPos;

  prvNh.param<std::string>("topic_move_to_inverse", topicMoveToInverse, "tcp/ctrl");
  prvNh.param<std::string>("topic_gripper_cs_active", topicGripperCsActive, "inverse/gripper_cs_active");
  prvNh.param<std::string>("topic_arm_reached_space_constraints", topicArmReachedSpaceContraints, "inverse/arm_reached_space_constraints");
  prvNh.param<std::string>("topic_joint_angle_inverse", topicJointAngleInverse, "/joint_visual");
  prvNh.param<std::string>("topic_arm_command", topicArmCommand, "arm_command");
  prvNh.param<std::string>("topic_predefined_pos", topicPredefinedPos, "inverse/predefined_pos");
  prvNh.param<double>("space_constraints", _maxSpaceConstrain, 1.22);

  _pubMoveToInverse = _nh.advertise<ohm_arm_interface::RobotarmCtrl>(topicMoveToInverse, 1);
  _pubReachedSpaceConstraints = _nh.advertise<std_msgs::Bool>(topicArmReachedSpaceContraints, 1);
  _pubGripperCsActive         = _nh.advertise<std_msgs::Bool>(topicGripperCsActive, 1);
  _subsArmCommand             = _nh.subscribe(topicArmCommand, 1, &ArmInterFace::callBackArmCommand, this);
  _subsJointAnglesInverse     = _nh.subscribe(topicJointAngleInverse, 1, &ArmInterFace::callBackJointAngles, this);

  _serverArmPredefinedCmd    = _nh.advertiseService(topicPredefinedPos, &ArmInterFace::callBackServicePreDefinedCmd, this);

  _signalToMoveArmInReal = false; //just leave for true now. Was a safety feature on the old node
  _armCommandLast = ros::Time::now();
  _timer = _nh.createTimer(ros::Duration(0.01), &ArmInterFace::callBackTimer, this);

  PredefinedArmPose posZero(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_ZERO);
  _predefinedPoses.push_back(posZero);

  PredefinedArmPose posInit(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_INITIAL);
  _predefinedPoses.push_back(posInit);

  PredefinedArmPose posDown(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_DOWN);
  _predefinedPoses.push_back(posDown);

  PredefinedArmPose posUp(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_UP);
  _predefinedPoses.push_back(posUp);

  PredefinedArmPose posRight(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_SIDE_RIGHT);
  _predefinedPoses.push_back(posRight);

  _callBackConfig = boost::bind(&ArmInterFace::callbackDynamicReconfigure, this, _1, _2);
  _serverReconf.setCallback(_callBackConfig);
}

ArmInterFace::~ArmInterFace()
{
  // TODO Auto-generated destructor stub
}

void ArmInterFace::callBackArmCommand(const ohm_teleop_msgs::ArmCommand& cmd)
{
  _comand = cmd;
  _armCommandLast = ros::Time::now();
}

void ArmInterFace::callBackJointAngles(const sensor_msgs::JointState& jsUpdate)
{
  _actualJointPosition.position.clear();

  _actualJointPosition.header.stamp = ros::Time::now();
  _actualJointPosition.header.frame_id = "";

  _actualJointPosition.position.push_back( jsUpdate.position.at(0));
  _actualJointPosition.position.push_back(-jsUpdate.position.at(1));
  _actualJointPosition.position.push_back(-jsUpdate.position.at(2));
  _actualJointPosition.position.push_back(jsUpdate.position.at(3)-M_PI); // gegen minus unendlich -js_update.position.at(3)-M_PI
  _actualJointPosition.position.push_back(jsUpdate.position.at(4));
  _actualJointPosition.position.push_back(jsUpdate.position.at(5));
  _actualJointPosition.position.push_back(jsUpdate.position.at(6));
  //Correction angle of joint7 - because of atan2 in ohm_inverse node
  if (_actualJointPosition.position.at(6) > M_PI)
  {
    _actualJointPosition.position.at(6) = _actualJointPosition.position.at(6) - 2*M_PI;
  }
  _actualJointPosition.position.push_back(jsUpdate.position.at(7));
  _actualJointPosition.position.push_back(jsUpdate.position.at(8));
}

void ArmInterFace::publishToInverse(void)
{

  ohm_arm_interface::RobotarmCtrl  currentCmd;

  currentCmd.theta_old.header.stamp = ros::Time::now();
  currentCmd.theta_old.header.frame_id = "";

  currentCmd.theta_old.name = _actualJointPosition.name;
  currentCmd.theta_old.position = _actualJointPosition.position;

  currentCmd.drei_rot.data = _newRotationMatrix;
  currentCmd.des_pos.data  = _relativePositionChange;
  currentCmd.gripper_cmd   = _actualJointPosition.position.at(7);
  currentCmd.amr_config = 0;
  if (_signalToMoveArmInReal) // has to be true that the arm moves in real
  {
    currentCmd.amr_config = 1;
  }
  //Publisher
  _pubMoveToInverse.publish(currentCmd);
}

void ArmInterFace::callBackTimer(const ros::TimerEvent& ev)
{
  if((ros::Time::now() - _armCommandLast).toSec() > 0.5)
  {
    ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " error! Command Timeout! Stop command");
    _comand.xe = 0.0;
    _comand.ye = 0.0;
    _comand.ze = 0.0;
    _comand.change_coordinate_system = false;
    _comand.initial_pose             = false;
    _comand.initial_up               = false;
    _comand.initial_look_down        = false;
    _comand.initial_tunnel_side      = false;
    _comand.initial_tunnel_up        = false;
    _comand.rplus                    = false;
    _comand.rminus                   = false;
    _comand.pplus                    = false;
    _comand.pminus                   = false;
    _comand.yplus                    = false;
    _comand.yminus                   = false;
    _comand.grauf                    = false;
    _comand.grzu                     = false;
    this->remapCommand(_comand);
    this->setNewGoalPosition();
    this->publishToInverse();
    return;
  }

  //  this->setCheckSpaceConstraints();
  //  if(_reachedSpaceConstrains)
  //  {
  //    _comand.xe = 0.0;
  //    _comand.ye = 0.0;
  //    _comand.ze = 0.0;
  //    _comand.change_coordinate_system = false;
  //    _comand.initial_pose             = false;
  //    _comand.initial_up               = false;
  //    _comand.initial_look_down        = false;
  //    _comand.initial_tunnel_side      = false;
  //    _comand.initial_tunnel_up        = false;
  //    _comand.rplus                    = false;
  //    _comand.rminus                   = false;
  //    _comand.pplus                    = false;
  //    _comand.pminus                   = false;
  //    _comand.yplus                    = false;
  //    _comand.yminus                   = false;
  //    _comand.grauf                    = false;
  //    _comand.grzu                     = false;
  //    this->publishToInverse();
  //    return;
  //  }

  this->remapCommand(_comand); // Joystick Input - Coordinate Fram and Change of Pose or Gripper
  this->setNewGoalPosition(); // Calculate new Position and Orientation Matrix

  //Check for zero angles position
  //  this->set_zero_position_from_remap(_comand);
  //  if (this->get_set_angle_to_zero_position() == true) {this->set_angle_to_zero_positon_false();}
  //
  //  // Check for start angles position
  if(_comand.initial_pose)
  {
    // this->setInitialPosition();
    this->setPredefinedPose(_predefinedPoses[static_cast<unsigned int>(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_ZERO)]);
    _signalToMoveArmInReal = true;
  }
  else if(_comand.initial_up)
  {
    this->setPredefinedPose(_predefinedPoses[static_cast<unsigned int>(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_INITIAL)]);
    _signalToMoveArmInReal = true;
  }
  else if(_comand.initial_look_down)
  {
    this->setPredefinedPose(_predefinedPoses[static_cast<unsigned int>(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_DOWN)]);
    _signalToMoveArmInReal = true;
  }
  else if(_comand.initial_tunnel_up)
  {
    this->setPredefinedPose(_predefinedPoses[static_cast<unsigned int>(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_UP)]);
    _signalToMoveArmInReal = true;
  }
  else if(_comand.initial_tunnel_side)
  {
    this->setPredefinedPose(_predefinedPoses[static_cast<unsigned int>(ohm_teleop_msgs::SetArmPredefinedPosRequest::CMD_SIDE_RIGHT)]);
    _signalToMoveArmInReal = true;
  }

  std_msgs::Bool armReachedSpaceConstraints;
  double space_maximum = std::sqrt(std::pow(_relativePositionChange.at(0), 2.0) + std::pow(_relativePositionChange.at(1), 2.0) + std::pow(_relativePositionChange.at(2), 2.0));
  const bool amEnde = this->checkSpaceConstraints();
  std::cout << __PRETTY_FUNCTION__ << "constraints " << amEnde << std::endl;
  if(amEnde)
  {
    _comand.xe = 0.0;
    _comand.ye = 0.0;
    _comand.ze = 0.0;
    _comand.change_coordinate_system = false;
    _comand.initial_pose             = false;
    _comand.initial_up               = false;
    _comand.initial_look_down        = false;
    _comand.initial_tunnel_side      = false;
    _comand.initial_tunnel_up        = false;
    _comand.rplus                    = false;
    _comand.rminus                   = false;
    _comand.pplus                    = false;
    _comand.pminus                   = false;
    _comand.yplus                    = false;
    _comand.yminus                   = false;
    _comand.grauf                    = false;
    _comand.grzu                     = false;
    this->remapCommand(_comand);
    this->setNewGoalPosition();
    this->publishToInverse();
    return;
  }
  if (space_maximum >= _maxSpaceConstrain)
    armReachedSpaceConstraints.data = true;
  else
    armReachedSpaceConstraints.data = false;
  _pubReachedSpaceConstraints.publish(armReachedSpaceConstraints);
  std::cout << __PRETTY_FUNCTION__ << "rel position change: ";
  for(auto& iter : _relativePositionChange)
    std::cout << iter << " ";
  std::cout << " abs : " << space_maximum << std::endl;

  std::cout << __PRETTY_FUNCTION__ << "rel rot change: ";
  for(auto& iter : _relativeRotationChange)
    std::cout << iter << " ";
  std::cout << std::endl;
  // Publish to ohm_inverse_node
  this->publishToInverse();
}

void ArmInterFace::remapCommand(const ohm_teleop_msgs::ArmCommand& cmd)
{
  if (cmd.change_coordinate_system == 1 && !_oldCoordinateSystem)
  {
    _changeCoordinateSystem = !_changeCoordinateSystem;
  }
  _oldCoordinateSystem = cmd.change_coordinate_system;


  if(_changeCoordinateSystem)
  {
    _coordianteSystem = 1.0;
  }

  else if(!_changeCoordinateSystem)
  {
    _coordianteSystem = 0.0;
  }

  //Check for coordinate system and then change the input of axes for global and local coordinate frame
  std_msgs::Bool gripperCsActive;
  if(_coordianteSystem < 0.1)
  {

    _relativePositionChange[0] = cmd.xe * _linearScalePosition;
    _relativePositionChange[2] = cmd.ze * _linearScalePosition;
    _relativePositionChange[1] = cmd.ye * _linearScalePosition;
    //   std::cout << __PRETTY_FUNCTION__ << -cmd.ze << " * " << _linearScalePosition << " = " << std::setprecision(5) << _relativePositionChange[0] << std::endl;
    gripperCsActive.data = false;
  }
  else// (coordiante_system == 1.0)
  {

    _relativePositionChange[0] = -cmd.ze * _linearScalePosition;
    _relativePositionChange[2] =  cmd.xe * _linearScalePosition;
    _relativePositionChange[1] =  cmd.ye * _linearScalePosition;
    //   std::cout << __PRETTY_FUNCTION__ << -cmd.ze << " * " << _linearScalePosition << " = " << std::setprecision(5) << _relativePositionChange[0] << std::endl;
    gripperCsActive.data = true;
  }


  _relativeRotationChange[0] = (cmd.rplus -cmd.rminus) * _linearScaleRotation;
  _relativeRotationChange[1] = (cmd.pplus -cmd.pminus) * _linearScaleRotation / 6.0;
  _relativeRotationChange[2] = (cmd.yplus -cmd.yminus) * _linearScaleRotation / 6.0;


  // Probably without use
  if(_actualJointPosition.position.at(7) < -1.6)
    _actualJointPosition.position.at(7) = -1.6;
  else if(_actualJointPosition.position.at(7) > 1.0)
    _actualJointPosition.position.at(7) = 1.0;
  else
    _actualJointPosition.position.at(7) += (cmd.grauf - cmd.grzu) * _linearScaleGripper;
  //  ROS_ERROR("Gripper des val %f \n", _actual_joint_position.position.at(7));
  //  std::cout << "gripper des val "  <<_actual_joint_position.position.at(7) << std::endl;
  // std_msgs::Bool armReachedSpaceConstraints;
  // armReachedSpaceConstraints.data = reached_space_constrains;
  //_arm_reached_space_constraints.publish(armReachedSpaceConstraints);
  //  std::cout << __PRETTY_FUNCTION__ << "pos change\n" << std::endl;
  //  for(auto& iter : _relativePositionChange)
  //    std::cout << std::setprecision(20) << iter << " " <<  std::endl;


  _pubGripperCsActive.publish(gripperCsActive);
}

void ArmInterFace::setNewGoalPosition()
{
  double convert_joints[7]    = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double convert_pd[3]        = {0.0, 0.0, 0.0};
  double convert_rd[3]        = {0.0, 0.0, 0.0};
  double convert_rd_3x3[9]    = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  std::copy(_relativePositionChange.begin(), _relativePositionChange.end(), convert_pd);
  std::copy(_relativeRotationChange.begin(), _relativeRotationChange.end(), convert_rd);
  std::copy(_actualJointPosition.position.begin(), _actualJointPosition.position.end()-2, convert_joints);

  Aktuelle_Position(convert_joints, convert_rd, convert_pd, _coordianteSystem, convert_rd_3x3, _maxSpaceConstrain);

  //  std::cout << __PRETTY_FUNCTION__ << "new rotation matrix:\n" << std::endl;
  for (size_t i = 0; i < _nrOfCmds; ++i)
  {
    //   std::cout << "\t" << convert_rd_3x3[i]  << std::endl;
    _newRotationMatrix.at(i) = convert_rd_3x3[i];
  }

  // std::cout << __PRETTY_FUNCTION__ << "position change:" << std::endl;
  for (size_t j = 0; j < _nrOfPos; ++j)
  {
    //   std::cout << "\t" << convert_pd[j] << std::endl;
    _relativePositionChange.at(j) = convert_pd[j];
  }
}

std::vector<double> ArmInterFace::getSelfcollisionDetectionPosition()
{
  if( ((_relativePositionChange.at(0) < _selfcollisionMax.at(0) ) &&
      (_relativePositionChange.at(0) > _selfcollisionMin.at(0))) &&
      ((_relativePositionChange.at(1) < _selfcollisionMax.at(1)) &&
          (_relativePositionChange.at(1) > _selfcollisionMin.at(1))))
  {
    if(_relativePositionChange.at(2) < 0.0)
    {
      _relativePositionChange.at(2) = 0.0;
      //   ROS_ERROR_STREAM("Be Careful - Self Collision - Type 1");
    }
  }

  if (((_relativePositionChange.at(0) < _selfcollisionMax.at(0)) &&
      (_relativePositionChange.at(0) > _selfcollisionMin.at(0))) &&
      ((_relativePositionChange.at(2) < _selfcollisionMax.at(2)) &&
          (_relativePositionChange.at(2) >= _selfcollisionMin.at(2))))
  {
    _relativePositionChange.at(0) = 0.377;
    // ROS_ERROR_STREAM("Be Careful - Self Collision - Type 2");
  }

  if (((_relativePositionChange.at(0) < _selfcollisionMax.at(0)) &&
      (_relativePositionChange.at(0) > _selfcollisionMin.at(0))) &&
      ((_relativePositionChange.at(2) < _selfcollisionMax.at(2)) &&
          (_relativePositionChange.at(2) >= _selfcollisionMin.at(2))) &&
          ((_relativePositionChange.at(1)<_selfcollisionMax.at(1)) &&
              (_relativePositionChange.at(1) > _selfcollisionMin.at(1))))
  {
    _relativePositionChange.at(1) = 0.0;
    //   ROS_ERROR_STREAM("Be Careful - Self Collision - Type 3");
  }

  return _relativePositionChange;
}

bool ArmInterFace::checkSpaceConstraints()
{
  double space_maximum = 0.0;
  space_maximum = std::sqrt(std::pow(_relativePositionChange.at(0), 2.0) + std::pow(_relativePositionChange.at(1), 2.0) + std::pow(_relativePositionChange.at(2), 2.0));

  if(space_maximum >= _maxSpaceConstrain)
  {
    ROS_ERROR(" Out of reach! - Reached Space Constraints");
    _reachedSpaceConstrains = true;
  }
  else
    _reachedSpaceConstrains = false;
  return _reachedSpaceConstrains;
}

void ArmInterFace::setInitialPosition(void)
{

  // if (init_zero_position.initial_pose == 1)
  {
    _signalToMoveArmInReal = true; //As long as variable is true - publisher to arm in ohm_inverse_node is on
    //  _changeAnglesToZeroPosition = true;
    _relativePositionChange = {0.377, 0.0, 0.0};
    _newRotationMatrix = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 };
    _actualJointPosition.position.at(0) = 0.0;
    _actualJointPosition.position.at(1) = 0.0;
    _actualJointPosition.position.at(2) = M_PI;
    _actualJointPosition.position.at(3) =-M_PI;
    _actualJointPosition.position.at(4) = 0.0;
    _actualJointPosition.position.at(5) = 0.0;
    _actualJointPosition.position.at(6) = 0.0;
    _actualJointPosition.position.at(7) = 0.0;
    _actualJointPosition.position.at(8) = 0.0;
  }
}

bool ArmInterFace::callBackServicePreDefinedCmd(ohm_teleop_msgs::SetArmPredefinedPos::Request& req, ohm_teleop_msgs::SetArmPredefinedPos::Response& res)
{
  PredefinedArmPose* predefined = NULL;
  for(auto& iter : _predefinedPoses)
    if(req.cmd == iter.type())
    {
      predefined = &iter;
      break;
    }
  if(!predefined)
  {
    std::cout << __PRETTY_FUNCTION__ << "Error. Unknown predefined pose " << static_cast<unsigned int>(req.cmd) << std::endl;
    return false;
  }


  //  switch(req.cmd)
  //  {
  //  case ohm_teleop_msgs::SetArmPredefinedPos::Request::CMD_ZERO:
  //  {
  //    predefined = &_predefinedPoses[static_cast<unsigned int>(req.cmd)];
  //    break;
  //  }
  //  case ohm_teleop_msgs::SetArmPredefinedPos::Request::CMD_INITIAL:
  //    predefined = &_predefinedPoses[static_cast<unsigned int>(req.cmd)];
  //    break;
  //  case ohm_teleop_msgs::SetArmPredefinedPos::Request::CMD_DOWN:
  //    break;
  //  case ohm_teleop_msgs::SetArmPredefinedPos::Request::CMD_UP:
  //    break;
  //  case ohm_teleop_msgs::SetArmPredefinedPos::Request::CMD_SIDE_LEFT:
  //    break;
  //  default:
  //  {
  //
  //  }
  //  }
  this->setPredefinedPose(*predefined);
  res.ret = req.cmd;
  return true;
}

void ArmInterFace::setPredefinedPose(const PredefinedArmPose& pos)
{
  _relativePositionChange = pos.relPositionChange();
  _newRotationMatrix = pos.rotationMatrix();
  _actualJointPosition = pos.jointPosition();
}

void ArmInterFace::callbackDynamicReconfigure(ohm_arm_interface::ReconfigureConfig& config, uint32_t level)
{
  _linearScalePosition = config.linear_scale_position;
  _linearScaleRotation = config.linear_scale_rotation;
  _linearScaleGripper  = config.linear_scale_gripper;
}
