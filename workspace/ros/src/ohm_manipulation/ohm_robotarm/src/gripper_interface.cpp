/*
 * gripper_interface.cpp
 *
 *  Created on: 25.02.2016
 *      Author: feesma44884
 */
#include <ohm_robotarm/gripper_interface.hpp>

/**
 * @Globale variables
 */
std::shared_ptr<ohm_robotarm::GripperInterface> ohm_robotarm::g_gripper_interface;

namespace ohm_robotarm
{

Joint::Joint(ros::NodeHandle& node_handle, const std::string& name) :
Drive::Drive(node_handle, name),
_position_target_old_value(0.0)
{
}

bool Joint::init(ros::NodeHandle& node_handle, const double update_rate) {
  //check if joint was initialized correctly
  if(_is_initialized == false) {
    ROS_ERROR("ohm_robotarm::gripper_interface: Failed to initialize %s!", _data->_name.c_str());
    return false;
  }

  //register state publisher of the drive
  _state_publisher  = node_handle.advertise<DriveState>(_data->_name + "/command", 10);

  //register command callback
  _state_subscriber = node_handle.subscribe<DriveState>(_data->_name + "/state", 10, &Joint::callback_state, this);

  //register update timer for sending drive state
  _update_timer = node_handle.createTimer(ros::Duration(1.0 / update_rate), &Joint::callback_update, this);

  //set id
  _tx_state.header.frame_id = "ohm_robotarm::gripper_interface";

  return true;
}

void Joint::callback_update(const ros::TimerEvent& event) {
  //round new position
  const double position_target_new  = round(_tx_state.position * 1000.0) * 0.001;

  //check if new position is not the same as the old target position
  if(position_target_new != _position_target_old_value) {
    //send new position data
    _tx_state.header.stamp  = ros::Time::now();
    _tx_state.position      = position_target_new;
    _state_publisher.publish(_tx_state);

    //position is now old
    _position_target_old_value = position_target_new;
  }
}

void Joint::callback_state(const DriveState::ConstPtr& current_state) {
  //update state of the drive
  _rx_state = (*current_state);

  //round position
  _rx_state.position  = round((*current_state).position * 1000.0) * 0.001;
}

void Joint::set_target_position(const double target_position) {
  _tx_state.position      = target_position;
  _tx_state.velocity      = 0.0;
  _tx_state.acceleration  = 0.0;
}

GripperInterface::GripperInterface(ros::NodeHandle& node_handle) :
_node_handle(node_handle),
_update_rate(100.0),
_gripper_joint_list(),
_time_stamp_last_call(),
_update_timer(),
_command_sub(),
_gripper_speed(0.0),
_gripper_position(0.0),
_gripper_position_min(0.0),
_gripper_position_max(0.0),
_gripper_max_speed(0.0)
{
}

GripperInterface::~GripperInterface(void) {
  //reset everything
  for(auto it : _gripper_joint_list) {
    it.reset();
  }
}

bool GripperInterface::init(void)
{
  //info
  ROS_INFO("ohm_robotarm::gripper_interface: Initializing...");

  //load update rate from parameter server
  if(!_node_handle.getParam("gripper_interface/update_rate", _update_rate)) {
    ROS_ERROR("ohm_robotarm::gripper_interface: Failed to load %s/gripper_interface/update_rate! Abort!",
             _node_handle.getNamespace().c_str());
    return false;
  }

  //init joints
  if(!init_joints())  return false;

  //create timer for update loop
  _update_timer = _node_handle.createTimer(ros::Duration(1.0 / _update_rate),
                                           &GripperInterface::callback_update, this);

  //subscribe
  _command_sub      = _node_handle.subscribe<std_msgs::Float64>("gripper_speed", 10,
                                                                &GripperInterface::callback_command, this);
  //get actual time stamp
  clock_gettime(CLOCK_MONOTONIC, &_time_stamp_last_call);

  return true;
}
bool GripperInterface::init_joints(void) {
  //variables
  unsigned int count = 0;
  std::vector<std::string>  gripper_joint_names;
  urdf::Model robotarm_urdf;

  //load params from server
  //load limits
  if(!_node_handle.getParam("gripper_interface/gripper_position_min", _gripper_position_min)) {
    ROS_WARN("ohm_robotarm:gripper_interface: Failed to load gripper joint names from %s/gripper_interface/gripper_position_min",
             _node_handle.getNamespace().c_str());
  }

  if(!_node_handle.getParam("gripper_interface/gripper_position_max", _gripper_position_max)) {
    ROS_WARN("ohm_robotarm:gripper_interface: Failed to load gripper joint names from %s/gripper_interface/gripper_position_max",
             _node_handle.getNamespace().c_str());
  }

  if(!_node_handle.getParam("gripper_interface/gripper_max_speed", _gripper_max_speed)) {
    ROS_WARN("ohm_robotarm:gripper_interface: Failed to load gripper joint names from %s/gripper_interface/gripper_max_speed",
             _node_handle.getNamespace().c_str());
  }

  //load gripper joint names
  if(!_node_handle.getParam("gripper_interface/joints", gripper_joint_names)) {
    ROS_WARN("ohm_robotarm:gripper_interface: Failed to load gripper joint names from %s/gripper_interface/joints",
             _node_handle.getNamespace().c_str());
  }

  //create gripper joints
  count = 0;
  for(auto name : gripper_joint_names) {
    //create joint
    _gripper_joint_list.push_back(std::shared_ptr<Joint>(new Joint(_node_handle, name)));

    //init joint
    if(!_gripper_joint_list[count++]->init(_node_handle, _update_rate)) {
      ROS_ERROR("ohm_robotarm::gripper_interface: Failed to initialize joint: %s", name.c_str());
      return false;
    }
  }

  return true;
}

void GripperInterface::callback_update(const ros::TimerEvent& event) {
  struct timespec time_stamp_current_call;

  //check passed time
  clock_gettime(CLOCK_MONOTONIC, &time_stamp_current_call);

  //calculate elapsed time
  const double time_elapsed   =   (double)(time_stamp_current_call.tv_sec - _time_stamp_last_call.tv_sec + (time_stamp_current_call.tv_nsec - _time_stamp_last_call.tv_nsec) / 1000000000.0);

  //check passed time
  clock_gettime(CLOCK_MONOTONIC, &_time_stamp_last_call);

  //check if there is a new command
  if(_gripper_speed != 0.0) {
    //move gripper
    _gripper_position += _gripper_speed * time_elapsed;

    unsigned int n = 0;
    for(auto joint : _gripper_joint_list) {
      //update position
      if(n % 2 == 1) {
        joint->set_target_position(_gripper_position);
      }
      else {
        joint->set_target_position(-_gripper_position);
      }
      n++;
    }

    //speed is 0
    _gripper_speed = 0.0;
  }

}

void GripperInterface::callback_command(const std_msgs::Float64::ConstPtr& gripper_speed) {
  _gripper_speed = (double)((*gripper_speed).data);

  //check for limits
  if(fabs(_gripper_speed) > _gripper_max_speed) {
    if(_gripper_speed > 0.0) {
      _gripper_speed = _gripper_max_speed;
    }
    else if(_gripper_speed < 0.0) {
      _gripper_speed = -_gripper_max_speed;
    }
  }
}

};

int main (int argc, char** argv) {
    //init ros system
    ros::init(argc, argv, "ohm_robotarm::manual_control_interface_node");
    ros::NodeHandle node_handle;

    //create controller
    ohm_robotarm::g_gripper_interface = std::shared_ptr<ohm_robotarm::GripperInterface>(
                                         new ohm_robotarm::GripperInterface(node_handle));

    //initialize robotarm controller
    ohm_robotarm::g_gripper_interface->init();

    //wait until node is shutdown
    ros::spin();

    //delete global instance
    ohm_robotarm::g_gripper_interface.reset();

    return 0;
}
