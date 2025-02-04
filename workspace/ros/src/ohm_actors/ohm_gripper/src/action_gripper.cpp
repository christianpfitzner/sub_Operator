/*
 * action_gripper.cpp
 *
 *  Created on: Apr 3, 2018
 *      Author: phil
 */

#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <std_msgs/Float32.h>
#include <iostream>
#include "ohm_actors_msgs/ActionGripperAction.h"
#include <string>
#include <dynamic_reconfigure/server.h>
#include "ohm_gripper/ReconfigureActionGripConfig.h"

static const float ANGLE_CLOSED = 0.38;
//static const float ANGLE_GRIPPED = 0.0;
//static const float ANGLE_OPEN = -1.0;

static const float STUCKDIFF = 0.05;
static const float ARRIVEDDIFF = 0.05;

static double _open = -1.0;

void callback(ohm_gripper::ReconfigureActionGripConfig& config, uint32_t level);

class ActionCmdGripper
{
public:
  ActionCmdGripper(const std::string& name):
    _as(_nh, name, false),
    _angleGripperLeft(0.0),
    _angleGripperRight(0.0),
    _angleGripperLastLeft(NAN),
    _angleGripperLastRight(NAN),
    _cbLeftReceived(false),
    _cbRightReceived(false),
    _initialMove(false)
{
    _subsAngleGripperLeft =  _nh.subscribe("gripper1/pos/present", 1, &ActionCmdGripper::callBackAngleGripperLeft , this);
    _subsAngleGripperRight = _nh.subscribe("gripper2/pos/present", 1, &ActionCmdGripper::callBackAngleGripperRight, this);
    _pubAngleGripperLeft = _nh.advertise<std_msgs::Float32>("/gripper1/pos/des", 1);
    _pubAngleGripperRight= _nh.advertise<std_msgs::Float32>("/gripper2/pos/des", 1);
    _as.registerGoalCallback(boost::bind(&ActionCmdGripper::goalCB, this));
    _as.start();
}
  void goalCB()
  {
    std::cout << __PRETTY_FUNCTION__ << " got command " << std::endl;
    _goal =  *_as.acceptNewGoal();
    std_msgs::Float32 cmdLeft;
    std_msgs::Float32 cmdRight;
    if(_goal.cmd == _goal.OPEN)
    {
      std::cout << __PRETTY_FUNCTION__ << " command open gripper " << std::endl;
      cmdLeft.data  =        _open;
      cmdRight.data = -1.0 * _open;
      std::cout << __PRETTY_FUNCTION__ << " command =  " << _open << std::endl;
      std::cout << __PRETTY_FUNCTION__ << " message " << cmdLeft.data << std::endl;
    }
    if(_goal.cmd == _goal.CLOSE)
    {
      std::cout << __PRETTY_FUNCTION__ << " command close gripper " << std::endl;
      cmdLeft.data  =        ANGLE_CLOSED;
      cmdRight.data = -1.0 * ANGLE_CLOSED;
      _initialMove = false;
    }
    _pubAngleGripperLeft.publish(cmdLeft);
    _pubAngleGripperRight.publish(cmdRight);
    usleep(1000 * 500);
    _angleGripperLastLeft = NAN;
    _angleGripperLastRight = NAN;
    _cbLeftReceived = false;
    _cbRightReceived = false;
    _timerMain = _nh.createTimer(ros::Duration(0.1), &ActionCmdGripper::timerCallBack, this);
  }
  void callBackAngleGripperLeft(const std_msgs::Float32& msg)
  {
    _cbLeftReceived = true;
    _angleGripperLeft = msg.data;
  }
  void callBackAngleGripperRight(const std_msgs::Float32& msg)
  {
    _cbRightReceived = true;
    _angleGripperRight = msg.data;
  }
  void timerCallBack(const ros::TimerEvent& ev)
  {
    float diffLeft = 0.0;
    float diffRight = 0.0;
    float deltaLeft = 0.0;
    float deltaRight = 0.0;
    ohm_actors_msgs::ActionGripperResult result;
    if(_goal.cmd == _goal.OPEN)
    {
      if(this->arrived(_open))
      {
        std::cout << __PRETTY_FUNCTION__ << " succesfully executed open command " << std::endl;
        result.result = ohm_actors_msgs::ActionGripperResult::SUCCESS;
        _timerMain.stop();
        _as.setSucceeded(result);
      }
    }
    else if(_goal.cmd == _goal.CLOSE)
    {
      this->close(result);
//      if(_cbLeftReceived && _cbRightReceived && !_initialMove)
//        _initialMove = !this->stuck();
//      else if(_cbLeftReceived && _cbRightReceived && this->stuck())
//      {
//        std_msgs::Float32 commandLeft;
//        commandLeft.data = _angleGripperLeft;
//        std_msgs::Float32 commandRight;
//        commandRight.data = _angleGripperRight;
//        _pubAngleGripperLeft.publish(commandLeft);
//        _pubAngleGripperRight.publish(commandRight);
//        std::cout << __PRETTY_FUNCTION__ << " gripped something. Hold position " << _angleGripperLeft << " " << _angleGripperRight << std::endl;
//        result.result = ohm_actors_msgs::ActionGripperResult::SUCCESS;
//        _timerMain.stop();
//        _as.setSucceeded(result);
//      }
//      if(this->arrived(ANGLE_CLOSED))
//      {
//        std_msgs::Float32 commandLeft;
//        commandLeft.data = _open;
//        std_msgs::Float32 commandRight;
//        commandRight.data = -1.0 * _open;
//        _pubAngleGripperLeft.publish(commandLeft);
//        _pubAngleGripperRight.publish(commandRight);
//        std::cout << __PRETTY_FUNCTION__ << " gripping failed. Open gripper " << std::endl;
//        result.result = ohm_actors_msgs::ActionGripperResult::FAILURE;
//        _timerMain.stop();
//        _as.setSucceeded(result);
//      }
    }
    if(_cbLeftReceived && _cbRightReceived)
    {
      _angleGripperLastLeft = _angleGripperLeft;
      _angleGripperLastRight = _angleGripperRight;
    }
    _cbLeftReceived = false;
    _cbRightReceived = false;
  }
  bool stuck(void)
  {
    const float diffLeft  = std::abs(_angleGripperLeft - _angleGripperLastLeft);
    const float diffRight = std::abs(_angleGripperRight - _angleGripperLastRight);
    if((diffLeft < STUCKDIFF) && (diffRight < STUCKDIFF))
    {
      std::cout << __PRETTY_FUNCTION__ << "diff l = " << _angleGripperLeft  << " - " <<  _angleGripperLastLeft << std::endl;
      return true;
    }
    else
      return false;
  }
  bool arrived(const float goalPos)
  {
    const float diffLeft = std::abs(goalPos - _angleGripperLeft);
    const float diffRight = std::abs(-1.0 * goalPos - _angleGripperRight);
    return ((diffLeft < ARRIVEDDIFF) && (diffRight < ARRIVEDDIFF));
  }
  void close(ohm_actors_msgs::ActionGripperResult& result)
  {
    if(!(_cbLeftReceived && _cbRightReceived))
    {
      std::cout << __PRETTY_FUNCTION__ << " no callbacks " << std::endl;
      return;
    }

    if(!_initialMove)
    {
      std::cout << __PRETTY_FUNCTION__ << " no initial move yet ?  " << std::endl;
      _initialMove = !this->stuck();
      std::cout << __PRETTY_FUNCTION__ << " initial move =  " << _initialMove << std::endl;
      return;
    }

    if(this->stuck())
    {
      std_msgs::Float32 commandLeft;
      commandLeft.data = _angleGripperLeft;
      std_msgs::Float32 commandRight;
      commandRight.data = _angleGripperRight;
      _pubAngleGripperLeft.publish(commandLeft);
      _pubAngleGripperRight.publish(commandRight);
      std::cout << __PRETTY_FUNCTION__ << " gripped something. Hold position " << _angleGripperLeft << " " << _angleGripperRight << std::endl;
      result.result = ohm_actors_msgs::ActionGripperResult::SUCCESS;
      _timerMain.stop();
      _as.setSucceeded(result);
    }
    if(this->arrived(ANGLE_CLOSED))
    {
      std_msgs::Float32 commandLeft;
      commandLeft.data = _open;
      std_msgs::Float32 commandRight;
      commandRight.data = -1.0 * _open;
      _pubAngleGripperLeft.publish(commandLeft);
      _pubAngleGripperRight.publish(commandRight);
      std::cout << __PRETTY_FUNCTION__ << " gripping failed. Open gripper " << std::endl;
      result.result = ohm_actors_msgs::ActionGripperResult::FAILURE;
      _timerMain.stop();
      _as.setSucceeded(result);
    }
  }
protected:
  ros::NodeHandle _nh;
  actionlib::SimpleActionServer<ohm_actors_msgs::ActionGripperAction> _as;
  ros::Subscriber _subsAngleGripperLeft;
  bool _cbLeftReceived;
  ros::Subscriber _subsAngleGripperRight;
  bool _cbRightReceived;
  ros::Publisher _pubAngleGripperLeft;
  ros::Publisher _pubAngleGripperRight;
  float _angleGripperLeft;
  float _angleGripperRight;
  ros::Timer _timerMain;
  ohm_actors_msgs::ActionGripperGoal _goal;
  float _angleGripperLastLeft;
  float _angleGripperLastRight;
  bool _initialMove;
};

void callback(ohm_gripper::ReconfigureActionGripConfig& config, uint32_t level)
{
  std_msgs::Float32 commandLeft;
  std_msgs::Float32 commandRight;
  _open = config.open_position;
  commandLeft.data = _open;
  commandRight.data = -1.0 * _open;
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "action_gripper");
  ActionCmdGripper action("pimmel");
  dynamic_reconfigure::Server<ohm_gripper::ReconfigureActionGripConfig> server;
  dynamic_reconfigure::Server<ohm_gripper::ReconfigureActionGripConfig>::CallbackType f;

  f = boost::bind(&callback, _1, _2);
  server.setCallback(f);
  ros::spin();
}


