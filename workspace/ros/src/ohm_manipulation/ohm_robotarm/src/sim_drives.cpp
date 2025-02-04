/*
 * simulated_drives.cpp
 *
 *  Created on: 04.02.2016
 *      Author: feesma44884
 */
#include <ohm_robotarm/sim_drives.hpp>

namespace ohm_robotarm
{

SimDrive::SimDrive(ros::NodeHandle& node_handle, const std::string& name) :
Drive(node_handle, name), _time_stamp_last_call()
{
}

bool SimDrive::init(ros::NodeHandle& node_handle, const double update_rate) {
  //check if sim drive was initialized correctly
  if(_is_initialized == false) {
    ROS_ERROR("ohm_robotarm::sim_drives: Failed to initialize %s!", _data->_name.c_str());
    return false;
  }

  //fill header
  _tx_state.header.frame_id = "ohm_robotarm::sim_drives";

  //register state publisher of the drive
  _state_publisher  = node_handle.advertise<DriveState>(_data->_name + "/state", 1);

  //register command callback
  _state_subscriber = node_handle.subscribe<DriveState>(_data->_name + "/command", 1, &SimDrive::callback_command, this);

  //register update timer for sending drive state
  _update_timer = node_handle.createTimer(ros::Duration(1.0 / update_rate), &SimDrive::callback_update, this);

  //get actual time stamp
  clock_gettime(CLOCK_MONOTONIC, &_time_stamp_last_call);

  return true;
}

void SimDrive::callback_update(const ros::TimerEvent& event) {

  //get current time stamp
  struct timespec time_stamp;
  clock_gettime(CLOCK_MONOTONIC, &time_stamp);

  //calculate delta time since last call
  const double delta_time = round((double)(time_stamp.tv_sec - _time_stamp_last_call.tv_sec + (time_stamp.tv_nsec - _time_stamp_last_call.tv_nsec)
                              / OHM_ROBOTARM_HCL_TIME_CONVERT) * 1000.0) * 0.001;

  //calculate delta position
  const double delta_position = floor((_rx_state.position - _tx_state.position) * 1000.0) * 0.001;

  //check for delta position
  if(fabs(delta_position) > 0) {
    //check for position control mode (no velocity and acceleration is given -> so set)
    if(_rx_state.velocity == 0.0) {
      _rx_state.velocity = _data->_velocity_limit;
    }
    if(_rx_state.acceleration == 0.0) {
      _rx_state.acceleration = _data->_acceleration_limit;
    }

    //check if accelerating or decelerating
    const double accel_factor   = floor((fabs(delta_position) - (_tx_state.velocity * _tx_state.velocity) / _rx_state.acceleration) * 1000.0) * 0.001;

    if(accel_factor > 0) {
      const double delta_velocity = floor(fabs(_rx_state.velocity - _tx_state.velocity) * 10000.0) * 0.0001;
      if(delta_velocity > 0) {
        _tx_state.velocity +=  _rx_state.acceleration * delta_time;
      }
      //range velocity
      else if(_tx_state.velocity > _rx_state.velocity) {
        _tx_state.velocity = _rx_state.velocity;
      }
      _tx_state.acceleration = _rx_state.acceleration;
    }
    else if(accel_factor < 0) {
      const double delta_velocity = floor(_tx_state.velocity * 10000.0) * 0.0001;
      if(delta_velocity > 0) {
        _tx_state.velocity -=  _rx_state.acceleration * delta_time;
      }
      //range velocity
      else {
        _tx_state.velocity = 0.0;
      }
      _tx_state.acceleration = -_rx_state.acceleration;
    }

    //check direction
    if(delta_position > 0) {
      //update position
      _tx_state.position +=  _tx_state.velocity * delta_time;
    }
    else {
      //update position
      _tx_state.position -=  _tx_state.velocity * delta_time;
    }

    //check if everything is in rage
    if(_data->_has_position_limits) {
      if(_tx_state.position > _data->_position_limit_max) {
        _tx_state.position = _data->_position_limit_max;
      }
      else if(_tx_state.position < _data->_position_limit_min) {
        _tx_state.position = _data->_position_limit_min;
      }
    }
  } else {
    _tx_state.velocity     = 0.0;
    _tx_state.acceleration = 0.0;
  }

  //get time stamp
  clock_gettime(CLOCK_MONOTONIC, &_time_stamp_last_call);

  //publish state of the drive
  _state_publisher.publish(_tx_state);
}

void SimDrive::callback_command(const DriveState::ConstPtr& target_state) {
  //update target position
  _rx_state.position      = (*target_state).position;
  _rx_state.velocity      = fabs((*target_state).velocity);
  _rx_state.acceleration  = fabs((*target_state).acceleration);

  //check limits
  if(_data->_has_position_limits) {
    if(_rx_state.position > _data->_position_limit_max) {
      _rx_state.position = _data->_position_limit_max;
    }
    else if(_rx_state.position < _data->_position_limit_min) {
      _rx_state.position = _data->_position_limit_min;
    }
  }

  if(_data->_has_velocity_limit) {
    if(_rx_state.velocity > _data->_velocity_limit) {
      _rx_state.velocity = _data->_velocity_limit;
    }
  }

  if(_data->_has_acceleration_limit) {
    if(_rx_state.acceleration > _data->_acceleration_limit) {
      _rx_state.acceleration = _data->_acceleration_limit;
    }
  }
}

SimDriveManager::SimDriveManager(void) :
_drive_list()
{
}

bool SimDriveManager::init(ros::NodeHandle& node_handle) {
  //parameter variables
  double update_rate  = 100.0;
  std::vector<std::string>  joint_names;

  ROS_INFO("ohm_robotarm::sim_drives: Initialize simulate drives...");

  //get joint names from namespace
  node_handle.getParam("robotarm_general/joints", joint_names);

  //check if data was found
  if(joint_names.size() == 0) {
    ROS_ERROR("ohm_robotarm::sim_drives: No joint names were found at %s/robotarm_general/joints",
        node_handle.getNamespace().c_str());
    return false;
  }

  //get state update frequency
  if(!node_handle.getParam("sim_drives/update_rate", update_rate)) {
    ROS_ERROR("ohm_robotarm::sim_drives: No %s/sim_drives/update_rate found!", node_handle.getNamespace().c_str());
    return false;
  }

  //create sim drives
  for(auto joint : joint_names) {
    //load joint data from parameter server
    std::vector<int>  joint_data;
    if(!node_handle.getParam("robotarm_general/" + joint, joint_data)) {
      ROS_ERROR("ohm_robotarm::sim_drives: No drive data found at %s/robotarm_general/%s", node_handle.getNamespace().c_str(), joint.c_str());
      return false;
    }

    //load only joints with correct joint type: sim drive
    if(static_cast<Drive::Type>(joint_data[Drive::DataField::DF_TYPE]) == Drive::T_SIM_DRIVE) {
      //create new drives
      _drive_list.push_back(std::shared_ptr<SimDrive>(new SimDrive(node_handle, joint)));

      //init joint
      const unsigned int id = _drive_list.size() - 1;
      if(!_drive_list[id]->init(node_handle, update_rate)) {
        ROS_ERROR("ohm_robotarm::sim_drives: Failed to initialize sim drive %s! Abort!", joint.c_str());
        return false;
      }
    }
  }

  ROS_INFO("ohm_robotarm::sim_drives: Simulated drive manager initialized");
  return true;
}

};

void SigintHandler(int sig)
{
  //warning
  ROS_INFO("ohm_robotarm::sim_drives: STRG + C detected... exiting...");

  //warning
  ROS_INFO("ohm_robotarm::sim_drives: Bye Bye");

  //release
  ohm_robotarm::SimDriveManager::instance()->release();

  //shutdown
  ros::shutdown();
}

const unsigned int get_num_drives(ros::NodeHandle& node_handle) {
  std::vector<std::string>  joint_names;
  unsigned int num_joints = 0;

  //get joint names from namespace
  node_handle.getParam("robotarm_general/joints", joint_names);

  //check if data was found
  if(joint_names.size() == 0) {
    ROS_ERROR("ohm_robotarm::ohm_sim_drives: No joint names were found in %s/robotarm_general/joints", node_handle.getNamespace().c_str());
    return 0;
  }

  //count number of joints from type = type
  for(auto joint : joint_names) {
    std::vector<int>  joint_data;

    //load data
    if(!node_handle.getParam("robotarm_general/" + joint, joint_data)) {
      ROS_ERROR("ohm_robotarm::ohm_sim_drives: No drive data found at %s/robotarm_general/%s", node_handle.getNamespace().c_str(), joint.c_str());
      return 0;
    }

    //check type
    if(static_cast<ohm_robotarm::Drive::Type>(joint_data[ohm_robotarm::Drive::DF_TYPE]) == ohm_robotarm::Drive::T_SIM_DRIVE) {
      num_joints++;
    }
  }

  return num_joints;
}

int main (int argc, char** argv) {
    //init ros system
    ros::init(argc, argv, "ohm_robotarm::sim_drives");
    ros::NodeHandle node_handle;

    //check if there are dynamixel drives
    if(get_num_drives(node_handle) == 0) {
      ROS_WARN("ohm_robotarm::sim_drives: No drives found! Exiting!");
      return 0;
    }

    //sigint handler
    signal(SIGINT, SigintHandler);

    if(!ohm_robotarm::SimDriveManager::instance()->init(node_handle)) {
      ohm_robotarm::SimDriveManager::instance()->release();
      return -1;
    }

    ROS_INFO("ohm_robotarm::sim_drives: Enter message loop...");

    //spin
    ros::spin();

    //delete singleton
    ohm_robotarm::SimDriveManager::instance()->release();

    return 0;
}
