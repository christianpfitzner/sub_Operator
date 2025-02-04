/*
 * PredefinedArmPose.cpp
 *
 *  Created on: Mar 25, 2019
 *      Author: phil
 */

#include "PredefinedArmPose.h"

PredefinedArmPose::PredefinedArmPose(const uint8_t& type):
_prvNh("~"),
_type(type)
{
  if(static_cast<unsigned int>(type) >= _prefixes.size())
    throw "Error! Unknown type selected";
  std::string preFix = _prefixes[static_cast<unsigned int>(type)];
  std::vector<double> relPosChange(3, NAN);
  _prvNh.getParam(preFix + "rel_pos_change", _relativePositionChange);

  std::cout << __PRETTY_FUNCTION__ << "relPoschange for " << preFix  << ": " << std::endl;
  for(auto& iter : _relativePositionChange)
    std::cout << iter << " ";
  std::cout << std::endl;

  _prvNh.getParam(preFix + "rotation_matrix", _rotationMatrix);
  for(auto& iter : _rotationMatrix)
    std::cout << iter << " ";
  std::cout << std::endl;

  std::vector<double> jointPositions(9, NAN);
  _prvNh.getParam(preFix + "joint_position", jointPositions);
  _jointPosition.name.push_back("joint_1");
  _jointPosition.position.push_back(jointPositions[0]);
  _jointPosition.name.push_back("joint_2");
  _jointPosition.position.push_back(jointPositions[1]);
  _jointPosition.name.push_back("joint_3");
  _jointPosition.position.push_back(jointPositions[2]);
  _jointPosition.name.push_back("joint_4");
  _jointPosition.position.push_back(jointPositions[3]);
  _jointPosition.name.push_back("joint_5");
  _jointPosition.position.push_back(jointPositions[4]);
  _jointPosition.name.push_back("joint_6");
  _jointPosition.position.push_back(jointPositions[5]);
  _jointPosition.name.push_back("joint_7");
  _jointPosition.position.push_back(jointPositions[6]);
  _jointPosition.name.push_back("gr1");
  _jointPosition.position.push_back(jointPositions[7]);
  _jointPosition.name.push_back("gr2");
  _jointPosition.position.push_back(jointPositions[8]);

  std::cout << __PRETTY_FUNCTION__ << " joint position" << std::endl;
  for(auto& iter : _jointPosition.position)
  {
    std::cout << iter << " ";
  }
  std::cout << std::endl;

}

PredefinedArmPose::~PredefinedArmPose()
{

}

