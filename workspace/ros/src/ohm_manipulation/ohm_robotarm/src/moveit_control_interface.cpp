/*
 * hardware_interface.cpp
 *
 *  Created on: 01.12.2015
 *      Author: feesma44884
 */
#include <ohm_robotarm/moveit_control_interface.h>
#include <ohm_robotarm/moveit_interface_control_loop.h>

namespace ohm_robotarm
{
    JointData::JointData(void) :
    _name(""), _position(0.0), _velocity(0.0), _effort(0.0), _has_position_limit(false), _position_min(0.0), _position_max(0.0),
    _position_command(0.0), _position_command_old(0.0), _state_publisher(), _state_subscriber()
    {

    }

    JointData::JointData(ros::NodeHandle& node_handle, const std::string& name, const bool has_position_limit, const double position_max, const double position_min) :
    _name(name), _position(0.0), _velocity(0.0), _effort(0.0), _has_position_limit(has_position_limit), _position_max(position_max), _position_min(position_min),
    _position_command(0.0), _position_command_old(0.0), _state_publisher(), _state_subscriber()
    {
      //register publisher and subscriber
      _state_publisher   = node_handle.advertise<ohm_robotarm::DriveState>(_name + "/command", 10);
      _state_subscriber  = node_handle.subscribe<ohm_robotarm::DriveState>(_name + "/state", 10, &JointData::update_state, this);
    }

    void JointData::update_state(const ohm_robotarm::DriveState::ConstPtr& data) {
      //update data
      _position = data->position;
      _velocity = data->velocity;
      _effort   = 0.0;
    }

    MoveItControlInterface::MoveItControlInterface(ros::NodeHandle& node_handle) :
    _node_handle(node_handle),
    _joint_state_interface(),
    _joint_position_interface(),
    _joint_data()
    {
        //initialize hardware interface
        init();

        ROS_INFO_NAMED(OHM_ROBOTARM_APP_NAME, "Loaded hardware interface");
    }

    MoveItControlInterface::~MoveItControlInterface(void) {
    }

    void MoveItControlInterface::init() {
        ROS_INFO_STREAM_NAMED(OHM_ROBOTARM_APP_NAME, "Initialize hardware interface from namespace: " << _node_handle.getNamespace());

        std::vector<std::string> joints;

        //get names of the joints
        _node_handle.getParam("moveit_interface/joints", joints);

        //check if joints have been found
        if(joints.size() == 0) {
            ROS_FATAL_STREAM_NAMED(OHM_ROBOTARM_APP_NAME, "No joints found on parameter server moveit_interface/joints for controller. Was the correct .yaml file loaded?"
                    << " Namespace: " << _node_handle.getNamespace());

            exit(-1);
        }

        //clear list
        _joint_data.clear();

        //initialize controllers
        for(unsigned int n = 0; n < joints.size(); n++) {
            ROS_DEBUG_STREAM_NAMED(OHM_ROBOTARM_APP_NAME, "Loading joint: " << joints[n]);

            //variables for position limit
            bool has_position_limits = false;
            double position_max = 0.0, position_min = 0.0;

            //check for limit
            _node_handle.getParam("robot_description_planning/joint_limits/" + joints[n] + "/has_position_limits", has_position_limits);

            //load limits if necessarry
            if(has_position_limits == true) {
              _node_handle.getParam("robot_description_planning/joint_limits/" + joints[n] + "/max_position", position_max);
              _node_handle.getParam("robot_description_planning/joint_limits/" + joints[n] + "/min_position", position_min);
            }

            //create joint
            _joint_data.push_back(boost::shared_ptr<JointData>(new JointData(_node_handle, joints[n], has_position_limits, position_max, position_min)));

            //create state interface
            _joint_state_interface.registerHandle(hardware_interface::JointStateHandle(_joint_data[n]->_name, &_joint_data[n]->_position,
                                                  &_joint_data[n]->_velocity, &_joint_data[n]->_effort));

            //create position joint interface
            _joint_position_interface.registerHandle(hardware_interface::JointHandle(_joint_state_interface.getHandle(_joint_data[n]->_name),
                    &_joint_data[n]->_position_command));
        }

        registerInterface(&_joint_state_interface);
        registerInterface(&_joint_position_interface);

    }

    void MoveItControlInterface::read(const ros::Duration& time_elapsed) {
    }

    void MoveItControlInterface::write(const ros::Duration& time_elapsed) {
        for(unsigned int n = 0; n < _joint_data.size(); n++) {
            //check if new position command received
            if(_joint_data[n]->_position_command != _joint_data[n]->_position_command_old) {
              ohm_robotarm::DriveState message;

              //header
              message.header.frame_id    = "ohm_robotarm::MoveItInterface";
              message.header.stamp.sec   = ros::Time::now().sec;
              message.header.stamp.nsec  = ros::Time::now().nsec;

              message.position  = _joint_data[n]->_position_command;
              message.velocity  = 0.0f;
              message.acceleration  = 0.0;

              //check for position limit
              if(_joint_data[n]->_has_position_limit) {
                //check for range
                if(message.position <= _joint_data[n]->_position_max && message.position >= _joint_data[n]->_position_min) {
                  _joint_data[n]->_state_publisher.publish(message);
                }
              }
              else {
                //publish without range check
                _joint_data[n]->_state_publisher.publish(message);
              }

              //position command is old
              _joint_data[n]->_position_command_old = _joint_data[n]->_position_command;
            }
        }
    }
}

int main(int argc, char** argv) {
    //init ROS
    ros::init(argc, argv, "ohm_robotarm::moveit_control_interface_node");
    ros::NodeHandle node_handle;

    //run ROS loop in a seperate thread to avoid blocking the main control thread
    ros::AsyncSpinner   spinner(2);
    spinner.start();

    //create hardware interface
    boost::shared_ptr<ohm_robotarm::MoveItControlInterface>   moveit_interface;

    //reset hardware interface
    moveit_interface.reset(new ohm_robotarm::MoveItControlInterface(node_handle));

    //start control loop
    ohm_robotarm::MoveItControlLoop    control_loop(node_handle, moveit_interface);

    //wait until node is shut down
    ros::waitForShutdown();

    return 0;
}


