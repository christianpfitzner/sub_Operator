/*
 * joint_state_publisher.hpp
 *
 *  Created on: 08.03.2016
 *      Author: feesma44884
 */

#ifndef JOINT_STATE_PUBLISHER_HPP_
#define JOINT_STATE_PUBLISHER_HPP_

/**
 * @brief Standard includes
 */
#include <ros/ros.h>
#include <signal.h>
#include <ohm_robotarm/DriveState.h>
#include <sensor_msgs/JointState.h>

/**
 * @brief Project specific includes
 */
#include <ohm_robotarm/drive.hpp>
#include <ohm_robotarm/singleton.hpp>

/***
 * @namespace ohm_robotarm
 *
 * @brief Namespace of the AutonOHM robotarm
 */
namespace ohm_robotarm
{
  /**
   * @class Joint
   *
   * @brief Class representing a joint
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

  private:

    /**
     * @brief Callback function which receives the state for the drive/joint
     *
     * @param state[in] State of the drive/joint
     */
    void callback_state(const DriveState::ConstPtr& state);
  };

  /**
   * @class JointStatePublisher
   *
   * @brief Class is used to publish the joint states into one message
   */
  class JointStatePublisher : public Singleton<JointStatePublisher>
  {
  friend class Singleton<JointStatePublisher>;
  public:
    /**
     * @brief Destructor
     */
    ~JointStatePublisher(void);

    /**
     * @brief Initializes the joint state publisher
     *
     * @param node_handle[in] ROS node handle
     *
     * @return true if everything is ok
     */
    bool init(ros::NodeHandle& node_handle);

  private:
    /**
     * @brief Constructor
     */
    JointStatePublisher(void);

    /**
     * @brief Callback function for publishing commands to the drive
     *
     * @param event[in] Timer event
     */
    void callback_update(const ros::TimerEvent& event);

    std::vector<std::shared_ptr<Joint>>       _joint_list;    //!< List with simulated drives
    sensor_msgs::JointState                   _joint_states;            //!< Sensor msgs with joint states
    ros::Publisher                            _joint_states_publisher;  //!< Publisher of joint states

    ros::Timer                                _update_timer;  //!< Update timer for publishing joint states
  };

};

#endif /* JOINT_STATE_PUBLISHER_HPP_ */
