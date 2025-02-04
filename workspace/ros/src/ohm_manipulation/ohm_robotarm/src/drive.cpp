/*
 * drive.cpp
 *
 *  Created on: 01.03.2016
 *      Author: feesma44884
 */
#include <ohm_robotarm/drive.hpp>

namespace ohm_robotarm
{
Drive::Data::Data(const std::string& name, const unsigned int id, const Type& type, const unsigned int hardware_id,
             const unsigned int encoder_resolution, const int direction, const int offset,
             const bool has_position_limits, const double position_limit_min, const double position_limit_max,
             const bool has_velocity_limit, const double velocity_limit, const bool has_acceleration_limit, const double acceleration_limit) :
_name(name),
_id(id),
_type(type),
_hardware_id(hardware_id),
_encoder_resolution(encoder_resolution),
_direction(direction),
_offset(offset),
_has_position_limits(has_position_limits),
_position_limit_min(position_limit_min),
_position_limit_max(position_limit_max),
_has_velocity_limit(has_velocity_limit),
_velocity_limit(velocity_limit),
_has_acceleration_limit(has_acceleration_limit),
_acceleration_limit(acceleration_limit)
{

}

Drive::Drive::Drive(ros::NodeHandle& node_handle, const std::string& name) :
_data(),
_state_subscriber(),
_state_publisher(),
_update_timer(),
_tx_state(),
_rx_state(),
_is_initialized(false)
 {
  //create node handle
  std::vector<int>  drive_data;
  bool has_position_limits = false, has_velocity_limit = false, has_acceleration_limit = false;
  double position_limit_max = 0.0, position_limit_min = 0.0, velocity_limit = 0.0, acceleration_limit = 0.0;

  //load data array
  node_handle.getParam("robotarm_general/" + name, drive_data);

  //check for size
  if(drive_data.size() == 0) {
    ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: No data found on %s/robotarm_general/%s", node_handle.getNamespace().c_str(), name.c_str());
    return;
  }
  else if(drive_data.size() != 6) {
    ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Data array has not 6 entries!\n\t| ID | Type | Hardware-ID | Resolution | Direction | Offset |");
    return;
  }

  //load joint limits

  //get data
  if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/has_position_limits", has_position_limits)) {
    ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/has_position_limits!",
              node_handle.getNamespace().c_str(), name.c_str());
    return;
  }

  if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/has_velocity_limits", has_velocity_limit)) {
    ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/has_velocity_limits!",
              node_handle.getNamespace().c_str(), name.c_str());
    return;
  }

  if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/has_acceleration_limits", has_acceleration_limit)) {
    ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/has_acceleration_limits!",
              node_handle.getNamespace().c_str(), name.c_str());
    return;
  }

  //check if to load limits
  if(has_position_limits) {
    if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/max_position", position_limit_max)) {
        ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/max_position!",
                  node_handle.getNamespace().c_str(), name.c_str());
        return;
    }
    if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/min_position", position_limit_min)) {
        ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/min_position!",
                  node_handle.getNamespace().c_str(), name.c_str());
        return;
    }
  }

  if(has_velocity_limit) {
    if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/max_velocity", velocity_limit)) {
        ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/max_velocity!",
                  node_handle.getNamespace().c_str(), name.c_str());
        return;
    }
  }

  if(has_acceleration_limit) {
    if(!node_handle.getParam("robot_description_planning/joint_limits/" + name + "/max_acceleration", acceleration_limit)) {
        ROS_ERROR("ohm_robotarm::Drive::load_data_from_param: Parameter not found: %s/joint_limits/%s/max_acceleration!",
                  node_handle.getNamespace().c_str(), name.c_str());
        return;
    }
  }

  //create data
  _data.reset(new ohm_robotarm::Drive::Data(name, drive_data[DataField::DF_ID], static_cast<ohm_robotarm::Drive::Type>(drive_data[DataField::DF_TYPE]),
              drive_data[DataField::DF_HARDWARE_ID], drive_data[DataField::DF_RESOLUTION], drive_data[DataField::DF_DIRECTION], drive_data[DataField::DF_OFFSET],
              has_position_limits, position_limit_min, position_limit_max, has_velocity_limit, fabs(velocity_limit), has_acceleration_limit, fabs(acceleration_limit)));

  //class is initialized
  _is_initialized = true;
}

Drive::Drive::~Drive(void) {
  //stop timer
  _update_timer.stop();
}

const double Drive::Drive::convert_inc_to_rad(const int value) {
  return (static_cast<double>(value) / _data->_encoder_resolution) * M_PI * 2.0;
}

const int Drive::Drive::convert_rad_to_inc(const double value) {
  return static_cast<int>((value / (2.0 * M_PI)) * _data->_encoder_resolution);
}

const std::string&    Drive::Drive::get_name(void)         {return _data->_name;}
const unsigned int    Drive::Drive::get_id(void)           {return _data->_id;}
const Drive::Type&    Drive::Drive::get_type(void)         {return _data->_type;}
const unsigned int    Drive::Drive::get_hardware_id(void)  {return _data->_hardware_id;}

const unsigned int    Drive::Drive::get_encoder_resolution(void)   {return _data->_encoder_resolution;}
const unsigned int    Drive::Drive::get_direction(void)            {return _data->_direction;}
const unsigned int    ohm_robotarm::Drive::Drive::get_offset(void) {return _data->_offset;}

const bool            Drive::Drive::get_has_position_limits(void)  {return _data->_has_position_limits;}
const double          Drive::Drive::get_position_limit_min(void)   {return _data->_position_limit_min;}
const double          Drive::Drive::get_position_limit_max(void)   {return _data->_position_limit_max;}

const bool            Drive::Drive::get_has_velocity_limit(void)   {return _data->_has_velocity_limit;}
const double          Drive::Drive::get_velocity_limit(void)       {return _data->_velocity_limit;}

const bool            Drive::Drive::get_has_acceleration_limit(void) {return _data->_has_acceleration_limit;}
const double          Drive::Drive::get_acceleration_limit(void)     {return _data->_acceleration_limit;}

const DriveState&     Drive::Drive::get_tx_state(void)               {return _tx_state;}
const DriveState&     Drive::Drive::get_rx_state(void)               {return _rx_state;}

const bool            Drive::Drive::get_is_initialized(void)         {return _is_initialized;}
}


