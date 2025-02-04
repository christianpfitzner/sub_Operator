/*
 * gripper_interface.hpp
 *
 *  Created on: 25.02.2016
 *      Author: feesma44884
 */

#ifndef OHM_ROBOTARM_GRIPPER_INTERFACE_HPP_
#define OHM_ROBOTARM_GRIPPER_INTERFACE_HPP_

/**
 * @brief Standard includes
 */
#include <memory>
#include <ros/ros.h>
#include <urdf/model.h>
#include <trac_ik/trac_ik.hpp>
#include <kdl_parser/kdl_parser.hpp>
#include <std_msgs/Float64.h>
#include <geometry_msgs/Pose.h>

/**
 * @brief Project specific includes
 */
#include <ohm_robotarm/DriveState.h>
#include <ohm_robotarm/drive.hpp>

/**
 * @namespace ohm_robotarm
 */
namespace ohm_robotarm
{

  /**
   * @class Joint
   *
   * @brief Class representing a joint for the interface
   */
  class Joint : public Drive::Drive
  {
  public:
    /**
     * @brief Constructor
     *
     * @param node_handle[in] Reference to the node handle
     * @param name[in] Name of the joint (needed to load config from param server)

     */
    Joint(ros::NodeHandle& node_handle, const std::string& name);

    /**
     * @brief Initializes the Joint
     *
     * @param node_handle[in] Reference to the node handle
     * @param update_rate[in] Update rate of the joint in Hz
     *
     * @return true if everything is fine
     */
    bool init(ros::NodeHandle& node_handle, const double update_rate);

    /**
     * @brief Sets a target position for the drive/joint
     *
     * @param target_position[in] Target position
     */
    void set_target_position(const double target_position);

  private:

    /**
     * @brief Callback function for publishing commands to the drive
     *
     * @param event[in] Timer event
     */
    void callback_update(const ros::TimerEvent& event);

    /**
     * @brief Callback function which receives the current state of the drive
     *
     * @param current_state[in] State
     */
    void callback_state(const DriveState::ConstPtr& current_state);

    double    _position_target_old_value;     //!< Old target position
  };

  /**
   * @class GripperInterface
   *
   * @brief Manual control interface of the robot arm
   *
   * This class is used to control the robotarm directly without use of MoveIt!
   *
   */
  class GripperInterface
  {
  public:

    /**
     * @brief Constructor of class
     */
    GripperInterface(ros::NodeHandle& node_handle);

    /**
     * @brief Destructor
     */
    ~GripperInterface(void);

    /**
     * @brief Initializes robotarm controller
     *
     * @return true if finished with no error
     */
    bool init(void);

  private:
    /**
     * @brief Loads the joints from the parameter server
     *
     * @return true if joints were loaded correct
     */
    bool init_joints(void);

    /**
     * @brief Callback function for update loop
     *
     * @param event[in] Event
     */
    void callback_update(const ros::TimerEvent& event);

    /**
     * @brief Callback function for gripper commands
     *
     * @param gripper_command[in]
     */
    void callback_command(const std_msgs::Float64::ConstPtr& gripper_speed);


    ros::NodeHandle&   _node_handle; //!< ROS node handle
    double             _update_rate; //!< Update rate of the class

    std::vector<std::shared_ptr<Joint>>     _gripper_joint_list;    //!< List with drives of the gripper

    struct timespec             _time_stamp_last_call;  //!< Saves the time stamp of the last call

    ros::Timer        _update_timer;          //!< Timer for update loop
    ros::Subscriber   _command_sub;           //!< Subscriber for gripper commands
    double            _gripper_speed;         //!< Gripper command

    double            _gripper_position;      //!< Saves the gripper position
    double            _gripper_position_min;  //!< Minimum gripper position
    double            _gripper_position_max;  //!< Maximum gripper position
    double            _gripper_max_speed;     //!< Speed of the gripper
  };

  extern std::shared_ptr<GripperInterface>   g_gripper_interface;   //!< Global instance of interface

};

#endif /* OHM_ROBOTARM_GRIPPER_INTERFACE_HPP_ */
