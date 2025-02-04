/*
 * joint_state_publisher.cpp
 *
 *  Created on: 08.03.2016
 *      Author: feesma44884
 */
#include <ohm_robotarm/joint_state_publisher.hpp>

namespace ohm_robotarm
{

  Joint::Joint(ros::NodeHandle& node_handle, const std::string& name) :
  Drive(node_handle, name)
  {
  }

  bool Joint::init(ros::NodeHandle& node_handle, const double update_rate) {
    //check if sim drive was initialized correctly
    if(_is_initialized == false) {
      ROS_ERROR("ohm_robotarm::joint_state_publisher: Failed to initialize %s!", _data->_name.c_str());
      return false;
    }

    //register command callback
    _state_subscriber = node_handle.subscribe<DriveState>(_data->_name + "/state", 10, &Joint::callback_state, this);

    return true;
  }

  void Joint::callback_state(const DriveState::ConstPtr& state) {
    _rx_state = *state;
  }

  JointStatePublisher::JointStatePublisher(void) :
  _joint_list(),
  _joint_states(),
  _joint_states_publisher(),
  _update_timer()
  {
  }

  JointStatePublisher::~JointStatePublisher(void) {
    //stop timers
    _update_timer.stop();
  }

  bool JointStatePublisher::init(ros::NodeHandle& node_handle) {
    //parameter variables
      double update_rate  = 100.0;
      std::vector<std::string>  joint_names;

      ROS_INFO("ohm_robotarm::joint_state_publisher: Initialize joints...");

      //get joint names from namespace
      node_handle.getParam("robotarm_general/joints", joint_names);

      //check if data was found
      if(joint_names.size() == 0) {
        ROS_ERROR("ohm_robotarm::joint_state_publisher: No joint names were found at %s/robotarm_general/joints",
                        node_handle.getNamespace().c_str());
        return false;
      }

      //get state update frequency
      if(!node_handle.getParam("joint_state_publisher/update_rate", update_rate)) {
        ROS_ERROR("ohm_robotarm::joint_state_publisher: No %s/joint_state_publisher/update_rate found!", node_handle.getNamespace().c_str());
        return false;
      }

      //init joint state header
      _joint_states.header.frame_id = "ohm_robotarm::joint_state_publisher";

      //create joints
      for(auto joint : joint_names) {
        //create new drives
        _joint_list.push_back(std::shared_ptr<Joint>(new Joint(node_handle, joint)));

        //init joint
        const unsigned int id = _joint_list.size() - 1;
        if(!_joint_list[id]->init(node_handle, update_rate)) {
          ROS_ERROR("ohm_robotarm::joint_state_publisher: Failed to initialize drive %s! Abort!", joint.c_str());
          return false;
        }

        //save name to joint state list
        _joint_states.name.push_back(_joint_list[id]->get_name());
        _joint_states.position.push_back(0.0);
        _joint_states.velocity.push_back(0.0);
        _joint_states.effort.push_back(0.0);
      }

      //register update timer for sending drive state
      _update_timer = node_handle.createTimer(ros::Duration(1.0 / update_rate), &JointStatePublisher::callback_update, this);

      //register joint state publisher
      _joint_states_publisher  = node_handle.advertise<sensor_msgs::JointState>("joint_states", 10);

      ROS_INFO("ohm_robotarm::joint_state_publisher: Joint state publisher initialized");

      return true;
  }

  void JointStatePublisher::callback_update(const ros::TimerEvent& event) {
    //update joint states
    for(auto joint : _joint_list) {
      _joint_states.position[joint->get_id()] = joint->get_rx_state().position;
      _joint_states.velocity[joint->get_id()] = joint->get_rx_state().velocity;
      _joint_states.effort[joint->get_id()]   = 0.0;
    }

    //publish joint states
    _joint_states.header.stamp  = ros::Time::now();
    _joint_states_publisher.publish(_joint_states);
  }

};

void SigintHandler(int sig)
{
  //warning
  ROS_INFO("ohm_robotarm::joint_state_publisher: STRG + C detected... exiting...");

  //warning
  ROS_INFO("ohm_robotarm::joint_state_publisher: Bye Bye");

  //release
  ohm_robotarm::JointStatePublisher::instance()->release();

  //shutdown
  ros::shutdown();
}

int main (int argc, char** argv) {
    //init ros system
    ros::init(argc, argv, "ohm_robotarm::joint_state_publisher");
    ros::NodeHandle node_handle;

    //sigint handler
    signal(SIGINT, SigintHandler);

    //init joint state publisher
    if(!ohm_robotarm::JointStatePublisher::instance()->init(node_handle)) {
      return -1;
    }

    ROS_INFO("ohm_robotarm::joint_state_publisher: Enter message loop...");

    //spin
    ros::spin();

    //release
    ohm_robotarm::JointStatePublisher::instance()->release();

    return 0;
}
