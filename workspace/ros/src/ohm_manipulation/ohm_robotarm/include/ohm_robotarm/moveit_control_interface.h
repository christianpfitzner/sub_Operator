/*
 * moveit_control_interface.hpp
 *
 *  Created on: 01.12.2015
 *      Author: feesma44884
 */

#ifndef MOVEIT_CONTROL_INTERFACE_H_
#define MOVEIT_CONTROL_INTERFACE_H_

#include <memory>
#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/robot_hw.h>
#include <controller_manager/controller_manager.h>
#include <joint_limits_interface/joint_limits.h>
#include <joint_limits_interface/joint_limits_rosparam.h>
#include <boost/scoped_ptr.hpp>

#include <ros/ros.h>
#include <ohm_robotarm/DriveState.h>

#define OHM_ROBOTARM_APP_NAME "ohm_robotarm::MoveIt!Interface:"

/**
 * @namespace ohm_robotarm
 *
 * @brief namespace of the AutonOHM robotarm
 */
namespace ohm_robotarm
{

/**
 * class JointData
 *
 * @brief Manages data of a joint
 */
class JointData
{
public:
  /**
   * @brief Constructor
   */
  JointData(void);

  /**
   * @brief Constructor
   *
   * @param node_handle[in] ROS node handle
   * @param name[in] Name of the joint
   * @param has_position_limit[in] True if has position limit
   * @param position_max[in] Maximum position of the joint
   * @param position_min[in] Minimum position of the joint
   */
  JointData(ros::NodeHandle& node_handle, const std::string& name,
            const bool has_position_limit, const double position_max, const double position_min);

  /**
   * @brief Callback of ROS subscriber
   *
   * @param data[in] Data received
   */
  void update_state(const ohm_robotarm::DriveState::ConstPtr& data);

  /***
   * Current data of the joint
   */
  const std::string   _name;     /***< Name of the joint */
  double              _position; /***< Position of the joint */
  double              _velocity; /***< Velocity of the joint */
  double              _effort;   /***< Effort of the joint */

  /**
   * Position limits
   */
  const bool          _has_position_limit;  /***< True has position limit */
  const double        _position_max;        /***< Maximum position of the joint*/
  const double        _position_min;        /***< Minimum position of the joint*/

  /**
   * Command data for the joint
   */
  double              _position_command;      /***< Position command */
  double              _position_command_old;  /***< Old position command */


  ros::Publisher  _state_publisher;
  ros::Subscriber _state_subscriber;
};

/**
 * @class MoveItControlInterface
 *
 * @brief Interface between MoveIt and the drive interfaces of the robotarm
 */
class MoveItControlInterface : public hardware_interface::RobotHW
{
public:
    /**
     * @brief Constructor for hardware interface
     *
     * @param node_handle[in]: ROS node handle
     */
    MoveItControlInterface(ros::NodeHandle& node_handle);

    /**
     * @brief Destructor
     */
    ~MoveItControlInterface(void);

    /**
     * @brief Initializes the hardware interface
     */
    virtual void init();

    /**
     * @brief Reads the data from the hardware
     *
     * @param elapsed_time[in]: Elapsed time since last call
     *
     */
    virtual void read(const ros::Duration& time_elapsed);

    /**
     * @brief Writes the command data to the hardware
     *
     * @param elapsed_time[in]: Elapsed time since last call
     */
    virtual void write(const ros::Duration& time_elapsed);
protected:

    /**
     * ROS node handle
     */
    ros::NodeHandle                             _node_handle;    /***< ROS node handle */

    /**
     * ROS control interfaces
     */
    hardware_interface::JointStateInterface     _joint_state_interface;      /***< State interface */
    hardware_interface::PositionJointInterface  _joint_position_interface;   /***< Position command interface */

    /**
     * Shared memory
     */
    std::vector<boost::shared_ptr<JointData>>   _joint_data;                  /***< Vector: Joint data */
};

};

#endif /* MOVEIT_CONTROL_INTERFACE_H_ */
