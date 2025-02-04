/*
 * elmo_manager.cpp
 *
 *  Created on: 02.02.2016
 *      Author: feesma44884
 */
#include <ohm_robotarm/elmo_drives.hpp>

bool pre_op_config(void) {return ohm_robotarm::ElmoDriveManager::instance()->pre_operational_config();}

namespace ohm_robotarm
{
  ElmoDrive::ElmoDrive(ros::NodeHandle& node_handle, const std::string& name, const bool reset_drive) :
  Drive(node_handle, name),
  _node(nullptr),
  _reset_drive(reset_drive),
  _position_actual_value(0),
  _position_target_old_value(0.0),
  _velocity_actual_value(0),
  _velocity_target_old_value(0.0),
  _new_command(false),
  _is_released(false)
  {
  }

  ElmoDrive::~ElmoDrive(void) {
    release();
  }

  bool ElmoDrive::init(ros::NodeHandle& node_handle, const double update_rate) {
    //check if elmo drive was initialized correctly
    if(_is_initialized == false) {
      ROS_ERROR("ohm_robotarm::elmo_drives: Failed to initialize %s!", _data->_name.c_str());
      return false;
    }

    //fill header
    _tx_state.header.frame_id = "ohm_robotarm::elmo_drives";

    //register state publisher of the drive
    _state_publisher  = node_handle.advertise<DriveState>(_data->_name + "/state", 1);

    //register command callback
    _state_subscriber = node_handle.subscribe<DriveState>(_data->_name + "/command", 1, &ElmoDrive::callback_command, this);



    //register update timer for sending drive state
    _update_timer = node_handle.createTimer(ros::Duration(1.0 / update_rate), &ElmoDrive::callback_update, this);
    _update_timer.stop();

    //adding drive to CANopen manager
    CANopen::IManager()->add_node(_data->_hardware_id);

    //get ptr to node
    _node  = CANopen::IManager()->get_node(_data->_hardware_id);

    //map PDOs
    _node->get_PDO(CANopen::COB_PDO_T_1)->register_object("Position actual value", (uint64_t*)&_position_actual_value);
    _node->get_PDO(CANopen::COB_PDO_T_1)->register_object("Velocity actual value", (uint64_t*)&_velocity_actual_value);

    //config trigger -> update rate
    //calculate update time
    double can_update_time  = (1.0 / update_rate) * 1000.0 * 0.75;
           can_update_time  = round(can_update_time) + static_cast<double>(_data->_hardware_id / 10);

    //check for minium time
    if(can_update_time < 20.0)  can_update_time = 20.0;

    //set time
    _node->get_PDO(CANopen::COB_PDO_T_1)->config_trigger(CANopen::PDO::ASYNC_TIMER, static_cast<uint16_t>(can_update_time));

    //class is not released
    _is_released = false;

    return true;
  }

  void ElmoDrive::release(void) {
    //check if class is already released
    if(_is_released)  return;

    //release class
    _update_timer.stop();

    _is_released = true;
  }

  bool ElmoDrive::pre_operational_config(void) {
    //config motion
    //check reset flag
    if(_reset_drive == true) {
      _node->send_SDO(CANopen::SDO::WRITE, "Drive mode", 1, 5);        //set drive mode to PTP
      _node->SDO_sleep(10);

      _node->send_SDO(CANopen::SDO::WRITE, "Acceleration", 1, abs(convert_rad_to_inc(_data->_acceleration_limit)));    //set acceleration
      _node->send_SDO(CANopen::SDO::WRITE, "Deceleration", 1, abs(convert_rad_to_inc(_data->_acceleration_limit)));    //set deceleration
      _node->send_SDO(CANopen::SDO::WRITE, "Speed absolute", 1, abs(convert_rad_to_inc(_data->_velocity_limit)));  //set speed

      //enable PDO
      _node->get_PDO(CANopen::COB_PDO_T_1)->enable();


      //enable drive
      _node->SDO_sleep(2000);
      _node->send_SDO(CANopen::SDO::WRITE, "Motor on", 1, 1);

      //wait 5sec to enable actuator
      _node->SDO_sleep(5000);
    }
    else {
      //enable PDO
      _node->get_PDO(CANopen::COB_PDO_T_1)->enable();
    }

    //start update loop
    _update_timer.start();

    return true;
  }

  void ElmoDrive::callback_update(const ros::TimerEvent& event) {
    //check if network is operational
    if(CANopen::IManager()->get_state() != CANopen::Manager::MNS_OPERATIONAL) return;
    //header
    _tx_state.header.stamp  = ros::Time::now();

    //convert data from INC to RAD
    _tx_state.position      = convert_inc_to_rad(_position_actual_value - _data->_offset) * _data->_direction;//(static_cast<double>(_position_actual) / 4000.0) * M_PI * 0.01;
    _tx_state.velocity      = convert_inc_to_rad(_velocity_actual_value) * _data->_direction;// / 4000.0) * M_PI * 0.01;
    _tx_state.acceleration  = 0.0;

    //send message
    _state_publisher.publish(_tx_state);

    //check if new command is received
    if(_new_command) {

      //check new position
      const double position_target_new_value = round(_rx_state.position * 1000.0) * 0.001;

      //check new velocity
      const double velocity_target_new_value = round(_rx_state.velocity * 1000.0) * 0.001;

      //check if new velocity is not equal to the old velocity
      if(velocity_target_new_value != _velocity_target_old_value) {

        //convert velocity
        const int32_t velocity_target_incremental_value = convert_rad_to_inc(velocity_target_new_value);

        //check speed
        if(velocity_target_incremental_value < 10) {
          _node->send_SDO(CANopen::SDO::WRITE, "Speed absolute", 1, 10);
        }
        else {
          //set velocity
          _node->send_SDO(CANopen::SDO::WRITE, "Speed absolute", 1, velocity_target_incremental_value);
        }

        _velocity_target_old_value = velocity_target_new_value;
      }

      //check if new position is not equal to the old position
      if(position_target_new_value != _position_target_old_value) {

        //convert to target position
        const int32_t position_target_incremental_value = convert_rad_to_inc(position_target_new_value * _data->_direction) + _data->_offset;

        //move to position
        _node->send_SDO(CANopen::SDO::WRITE, "Position absolute", 1, position_target_incremental_value);
        _node->send_SDO(CANopen::SDO::WRITE, "Begin motion", 1, 1);

        _position_target_old_value = position_target_new_value;
      }

      //new command received
      _new_command  = false;
    }
  }

  void ElmoDrive::callback_command(const DriveState::ConstPtr& target_state) {
    //check if network is operational
    if(CANopen::IManager()->get_state() != CANopen::Manager::MNS_OPERATIONAL) return;

    //get position
    _rx_state.position  = (*target_state).position;

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

     //new command received
     _new_command = true;
  }

bool ElmoDriveManager::callBackServiceReset(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res)
{
    for(auto drive : _drive_list) 
    {
      drive->setResetDrives(true);
      if(!drive->pre_operational_config())
      {
        ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " somesing went wrong schnoeff schnoeff " << std::endl);
        return false;
      }  
        
    }

  std::cout << __PRETTY_FUNCTION__ << " huhuhu " << std::endl;
 return true; 
}


  ElmoDriveManager::ElmoDriveManager(void) :
  _drive_list()
  {

  }

  bool ElmoDriveManager::init(ros::NodeHandle& node_handle, const bool reset_drives) {
    //data
    double                    update_rate = 500.0;
    double                    canopen_update_rate = 500.0;
    std::vector<std::string>  joint_names;

    ROS_INFO("ohm_robotarm::elmo_drives: Initializing Elmo-MC drives of robot arm...");


    //get joint names from namespace
    node_handle.getParam("robotarm_general/joints", joint_names);

    //check if data was found
    if(joint_names.size() == 0) {
      ROS_ERROR("ohm_robotarm::elmo_drives: No joint names were found in %s/robotarm_general/joints", node_handle.getNamespace().c_str());
      return false;
    }

    //get state update rate
    if(!node_handle.getParam("elmo_drives/update_rate", update_rate)) {
      ROS_ERROR("ohm_robotarm::elmo_drives: No %s/elmo_drives/update_rate found!", node_handle.getNamespace().c_str());
      return false;
    }

    //get canopen update rate
    if(!node_handle.getParam("elmo_drives/canopen_update_rate", canopen_update_rate)) {
      ROS_ERROR("ohm_robotarm::elmo_drives: No %s/elmo_drives/update_rate found!", node_handle.getNamespace().c_str());
      return false;
    }

    //initializing can-open-manager
    CANopen::IManager()->init(node_handle, -1.0, reset_drives, canopen_update_rate);

    //register pre operational config function
    CANopen::IManager()->register_pre_operational_config_function(&pre_op_config);

    //register objects to object dictionary
    CANopen::IOD()->add(0x3002, "Acceleration", 32);
    CANopen::IOD()->add(0x3020, "Begin motion", 8);
    CANopen::IOD()->add(0x3050, "Deceleration", 32);
    CANopen::IOD()->add(0x3214, "Drive mode", 0);
    CANopen::IOD()->add(0x3146, "Motor on", 0);
    CANopen::IOD()->add(0x3186, "Position absolute", 32);
    CANopen::IOD()->add(0x31E3, "Speed absolute", 32);

    //create elmo drives
    for(auto joint : joint_names) {
      //load joint data from parameter server
      std::vector<int>  joint_data;
      if(!node_handle.getParam("robotarm_general/" + joint, joint_data)) {
        ROS_ERROR("ohm_robotarm::elmo_drives: No drive data found at %s/robotarm_general/%s", node_handle.getNamespace().c_str(), joint.c_str());
        return false;
      }

      //load only joints with correct joint type: sim drive
      if(static_cast<Drive::Type>(joint_data[Drive::DataField::DF_TYPE]) == Drive::T_ELMO_DRIVE) {
        //create new drives
        _drive_list.push_back(std::shared_ptr<ElmoDrive>(new ElmoDrive(node_handle, joint, reset_drives)));

        //init drive
        const unsigned int id = _drive_list.size() - 1;
        if(!_drive_list[id]->init(node_handle, update_rate)) {
          ROS_ERROR("ohm_robotarm::elmo_drives: Failed to initialize elmo drive %s! Abort!", joint.c_str());
          return false;
        }
      }
    }
    _serverReset = node_handle.advertiseService("arm/reset", &ElmoDriveManager::callBackServiceReset, this);
    return true;
  }

  bool ElmoDriveManager::pre_operational_config(void) {
    for(auto drive : _drive_list) {
      if(!drive->pre_operational_config())  return false;
    }
    return true;
  }

  const size_t ElmoDriveManager::get_num_drives(void) {
    return _drive_list.size();
  }
}



void SigintHandler(int sig)
{
  //warmomg
  ROS_INFO("ohm_robotarm::elmo_drives: STRG + C detected... exiting...");

  //warning
  ROS_INFO("ohm_robotarm::elmo_drives: Bye Bye");

  //release
  CANopen::IManager()->release();
  ohm_robotarm::ElmoDriveManager::instance()->release();

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
    ROS_ERROR("ohm_robotarm::elmo_drives: No joint names were found in %s/robotarm_general/joints", node_handle.getNamespace().c_str());
    return 0;
  }

  //count number of joints from type = type
  for(auto joint : joint_names) {
    std::vector<int>  joint_data;

    //load data
    if(!node_handle.getParam("robotarm_general/" + joint, joint_data)) {
      ROS_ERROR("ohm_robotarm::elmo_drives: No drive data found at %s/robotarm_general/%s", node_handle.getNamespace().c_str(), joint.c_str());
      return 0;
    }

    //check type
    if(static_cast<ohm_robotarm::Drive::Type>(joint_data[ohm_robotarm::Drive::DF_TYPE]) == ohm_robotarm::Drive::T_ELMO_DRIVE) {
      num_joints++;
    }
  }

  return num_joints;
}



int main (int argc, char** argv) {
    //wait before init
    sleep(2.0);

    //init ros system
    ros::init(argc, argv, "ohm_robotarm::elmo_drives");
    ros::NodeHandle nh("~");
    ros::NodeHandle node_handle;

    //check if there are elmo drives
    if(get_num_drives(node_handle) == 0) {
      ROS_WARN("ohm_robotarm::elmo_drives: No drives found! Exiting!");
      return 0;
    }

    //get execute params
    bool reset_drives = false;
    nh.getParam("reset_drives", reset_drives);

    //if drives will be reset -> send warning
    if(reset_drives) {
      ROS_ERROR("ohm_robotarm::elmo_drives: Drives will be reset! Check if a released drive will not injury someone!");
      //wait ->
      const unsigned int time = 2;
      for(auto n = 0; n < time; n++) {
        sleep(1);
        ROS_ERROR("ohm_robotarm::elmo_drives: Releasing drives in %i seconds...", (time - n));
      }
    }

    //sigint handler
    signal(SIGINT, SigintHandler);

    //init elmo manager
    if(!ohm_robotarm::ElmoDriveManager::instance()->init(node_handle, reset_drives)) {
      ohm_robotarm::ElmoDriveManager::instance()->release();
      return -1;
    }

    //check if drives where registered
    if(ohm_robotarm::ElmoDriveManager::instance()->get_num_drives() == 0) {
      ROS_WARN("ohm_robotarm::elmo_drives: No drives found on param server!");
      ohm_robotarm::ElmoDriveManager::instance()->release();
      return 0;
    }

    //spin
    ros::spin();

    //release
    ohm_robotarm::ElmoDriveManager::instance()->release();

    return 0;
}



