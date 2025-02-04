/*
 * dynamixel_drives.cpp
 *
 *  Created on: 11.02.2016
 *      Author: feesma44884
 */

/*
 * elmo_manager.cpp
 *
 *  Created on: 02.02.2016
 *      Author: feesma44884
 */
#include <ohm_robotarm/dynamixel_drives.hpp>

namespace ohm_robotarm
{
  DynamixelDrive::DynamixelDrive(ros::NodeHandle node_handle, const std::string& name) :
  Drive(node_handle, name),
  _position_actual(_data->_offset),
  _position_old(0.0),
  _new_command(false)
  {

  }

  DynamixelDrive::~DynamixelDrive(void) {
    _update_timer.stop();
  }

  bool DynamixelDrive::init(ros::NodeHandle& node_handle, const double update_rate) {
    //check if dynamixel drive was initialized correctly
    if(_is_initialized == false) {
      ROS_ERROR("ohm_robotarm::dynamixel_drives: Failed to initialize %s!", _data->_name.c_str());
      return false;
    }

    //fill header
    _tx_state.header.frame_id = "ohm_robotarm::dynamixel_drives";

    //register state publisher of the drive
    _state_publisher  = node_handle.advertise<DriveState>(_data->_name + "/state", 1);

    //register command callback
    _state_subscriber = node_handle.subscribe<DriveState>(_data->_name + "/command", 1, &DynamixelDrive::callback_command, this);

    //register update timer for sending drive state
    _update_timer = node_handle.createTimer(ros::Duration(1.0 / update_rate), &DynamixelDrive::callback_update, this);

    return true;
  }

  void DynamixelDrive::callback_update(const ros::TimerEvent& event) {
    //update actual state of the drive
    update_state();

    //send commands to drive
    send_commands();
  }

  void DynamixelDrive::callback_command(const DriveState::ConstPtr& target_state) {
    _rx_state = *target_state;


    //check position limits
    if(_data->_has_position_limits) {
      if(_rx_state.position > _data->_position_limit_max) {
        _rx_state.position = _data->_position_limit_max;
      }
      else if(_rx_state.position < _data->_position_limit_min) {
        _rx_state.position = _data->_position_limit_min;
      }
    }

    //get velocity
    _rx_state.velocity = fabs((*target_state).velocity);

    //check for velocity limits
    if(_data->_has_velocity_limit) {
      if(_rx_state.velocity > _data->_velocity_limit) {
        _rx_state.velocity = _data->_velocity_limit;
      }
    }

    //check if velocity is 0 -> then set maximum vel
    if(_rx_state.velocity == 0.0) {
      _rx_state.velocity = _data->_velocity_limit;
    }

    _new_command = true;
  }


  void DynamixelDrive::update_state(void) {
    //instance of drive manager
    std::shared_ptr<DynamixelDriveManager>  instance  = DynamixelDriveManager::instance();

    //define command
    const unsigned char    tx_buffer[4]   = {(unsigned char)(_data->_hardware_id), 0x24, '\r', '\n'};

    //send command
    instance->get_VCP().transmitBuffer(tx_buffer, 4);

    //wait for answer
    unsigned char          rx_buffer[16];
    unsigned int           rx_buffer_size;
    if(instance->get_VCP().receiveBuffer(rx_buffer, rx_buffer_size, 16, 100) != RS232::SUCCESS) {
      ROS_WARN("ohm_robotarm::dynamixel_drives: Dynamixel-ID: %i: No position data received!", _data->_hardware_id);
    }

    //check if an error is received
    if(rx_buffer[0] == 0) {
      ROS_WARN("ohm_robotarm::dynamixel_drives: Dynamixel-ID: %i: Error receiving position!", _data->_hardware_id);
      return;
    }

    //check if ID is correct
    uint8_t id = (uint8_t)(rx_buffer[0]);
    if(id == _data->_hardware_id) {
      //update position
      _position_actual  = (int16_t)(rx_buffer[1] << 8 | rx_buffer[2]);
    }

    _tx_state.header.stamp  = ros::Time::now();
    _tx_state.position      = convert_inc_to_rad(_position_actual - _data->_offset) * _data->_direction;
    _tx_state.velocity      = 0.0;
    _tx_state.acceleration  = 0.0;
    _state_publisher.publish(_tx_state);
  }

  void DynamixelDrive::send_commands(void) {
    //check if new command received
    if(_new_command)
    {
      //instance of drive manager
      std::shared_ptr<DynamixelDriveManager>  instance  = DynamixelDriveManager::instance();

      //round position
      const double new_position  = round(_rx_state.position * 1000.0) * 0.001;

      //check if new position is not the old one
      if(new_position != _position_old) {
        //calculate target position
        int16_t target_position = convert_rad_to_inc(new_position * (double)(_data->_direction)) + _data->_offset;

        //define command
        const unsigned char tx_buffer[6]      = {(unsigned char)(_data->_hardware_id), 0x1E, (unsigned char)(target_position>>8), (unsigned char)(target_position), '\r', '\n'};

        //send command
        instance->get_VCP().transmitBuffer(tx_buffer, 6);

        //wait for response
        unsigned char          rx_buffer[16];
        unsigned int           rx_buffer_size;
        if(instance->get_VCP().receiveBuffer(rx_buffer, rx_buffer_size, 16, 100) != RS232::SUCCESS) {
         ROS_WARN("ohm_robotarm::dynamixel_drives: Dynamixel-ID: %i: Failed to send command to set target position! No response from dynamixel drive!", _data->_hardware_id);
          return;
        }

        //check response
        if(!rx_buffer[0]) {
          ROS_WARN("ohm_robotarm::dynamixel_drives: Dynamixel-ID: %i: Failed to send command to set target position! Error received!", _data->_hardware_id);
        }

        _position_old = new_position;
      }

      _new_command = false;
    }
  }

  DynamixelDriveManager::DynamixelDriveManager(void) :
  _vcp(),
  _dynamixel_drive_list()
  {
  }

  bool DynamixelDriveManager::init(ros::NodeHandle& node_handle, const std::string serial_port_path) {
    //data
    double                    update_rate = 100.0;
    std::vector<std::string>  joint_names;

    ROS_INFO("ohm_robotarm::dynamixel_drives: Initializing Dynamixel drives of robot arm...");

    //get joint names from namespace
    node_handle.getParam("robotarm_general/joints", joint_names);

    //check if data was found
    if(joint_names.size() == 0) {
      ROS_ERROR("ohm_robotarm::dynamixel_drives: No joint names were found in %s/robotarm_general/joints", node_handle.getNamespace().c_str());
      return false;
    }

    //get state update rate
    if(!node_handle.getParam("dynamixel_drives/update_rate", update_rate)) {
      ROS_ERROR("ohm_robotarm::dynamixel_drives: No %s/dynamixel_drives/update_rate found!", node_handle.getNamespace().c_str());
      return false;
    }

    //connecting to VCP
    ROS_INFO("ohm_robotarm::dynamixel_drives: Connecting to serial port: %s", (char*)(serial_port_path.c_str()));

    //try to open serial com port
    if(!_vcp.connect(serial_port_path, RS232::B_38400)) {
        ROS_ERROR("ohm_robotarm::dynamixel_drives: Failed to open serial port (%s). Check if you have access rights!", serial_port_path.c_str());
        return false;
    }

    ROS_INFO("ohm_robotarm::dynamixel_drives: Connection established");

    //create dynamixel drives
    for(auto joint : joint_names) {
      //load joint data from parameter server
      std::vector<int>  joint_data;
      if(!node_handle.getParam("robotarm_general/" + joint, joint_data)) {
        ROS_ERROR("ohm_robotarm::dynamixel_drives: No drive data found at %s/robotarm_general/%s", node_handle.getNamespace().c_str(), joint.c_str());
        return false;
      }

      //load only joints with correct joint type: sim drive
      if(static_cast<Drive::Type>(joint_data[Drive::DataField::DF_TYPE]) == Drive::T_DYNAMIXEL_DRIVE) {
        //create new drives
        _dynamixel_drive_list.push_back(std::shared_ptr<DynamixelDrive>(new DynamixelDrive(node_handle, joint)));
      }
    }

    //init joints
    for(auto joint : _dynamixel_drive_list) {
      if(!joint->init(node_handle, update_rate)) {
        ROS_ERROR("ohm_robotarm::dynamixel_drives: Failed to initialize dynamixel drive %s! Abort!", joint->get_name().c_str());
        return false;
      }
    }

    ROS_INFO("ohm_robotarm::dynamixel_drives: Dynamixel drive manager initialized");

    return true;
  }

  const size_t DynamixelDriveManager::get_num_drives(void) {
    return _dynamixel_drive_list.size();
  }

};

void SigintHandler(int sig)
{
  //warning
  ROS_INFO("ohm_robotarm::dynamixel_drives: STRG + C detected... exiting...");

  //warning
  ROS_INFO("ohm_robotarm::dynamixel_drives: Bye Bye");

  //release manager
  ohm_robotarm::DynamixelDriveManager::instance()->release();

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
    ROS_ERROR("ohm_robotarm::dynamixel_drives: No joint names were found in %s/robotarm_general/joints", node_handle.getNamespace().c_str());
    return 0;
  }

  //count number of joints from type = type
  for(auto joint : joint_names) {
    std::vector<int>  joint_data;

    //load data
    if(!node_handle.getParam("robotarm_general/" + joint, joint_data)) {
      ROS_ERROR("ohm_robotarm::dynamixel_drives: No drive data found at %s/robotarm_general/%s", node_handle.getNamespace().c_str(), joint.c_str());
      return 0;
    }

    //check type
    if(static_cast<ohm_robotarm::Drive::Type>(joint_data[ohm_robotarm::Drive::DF_TYPE]) == ohm_robotarm::Drive::T_DYNAMIXEL_DRIVE) {
      num_joints++;
    }
  }

  return num_joints;
}

int main (int argc, char** argv) {
    //wait before init
    sleep(2.0);

    //init ros system
    ros::init(argc, argv, "ohm_robotarm::dynamixel_drives");
    ros::NodeHandle nh("~");
    ros::NodeHandle node_handle;

    //check if there are dynamixel drives
    if(get_num_drives(node_handle) == 0) {
      ROS_WARN("ohm_robotarm::dynamixel_drives: No drives found! Exiting!");
      return 0;
    }

    //get parameters
    std::string serial_port_path;
    nh.param<std::string>("serialPortPath", serial_port_path, "/dev/ttyACM0");

    //sigint handler
    signal(SIGINT, SigintHandler);

    //init dynamixel drive manager
    if(!ohm_robotarm::DynamixelDriveManager::instance()->init(node_handle, serial_port_path)) {
      ohm_robotarm::DynamixelDriveManager::instance()->release();
      return -1;
    }

    //check if drives where registered
    if(ohm_robotarm::DynamixelDriveManager::instance()->get_num_drives() == 0) {
      ROS_WARN("ohm_dynamixel_drives: No dynamixel drives found on param server!");
      ohm_robotarm::DynamixelDriveManager::instance()->release();
      return 0;
    }

    //spin
    ros::spin();

    //release
    ohm_robotarm::DynamixelDriveManager::instance()->release();

    return 0;
}

