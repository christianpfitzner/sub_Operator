/*
 * sim_drives.hpp
 *
 *  Created on: 02.02.2016
 *      Author: feesma44884
 */

#ifndef SIM_DRIVES_HPP_
#define SIM_DRIVES_HPP_

/**
 * @brief Definitions
 */
#define OHM_ROBOTARM_HCL_TIME_CONVERT    1000000000.0

/**
 * @brief Standard includes
 */
#include <ros/ros.h>
#include <signal.h>
#include <time.h>
#include <ohm_robotarm/DriveState.h>

/**
 * @brief Project specific includes
 */
#include <ohm_robotarm/drive.hpp>
#include <ohm_robotarm/singleton.hpp>

/**
 * @namespace ohm_robotarm
 */
namespace ohm_robotarm
{
  class SimDrive : public Drive::Drive
  {
  public:
    /**
     * @brief Constructor
     *
     * @param node_handle[in] Reference to the node handle
     * @param name[in] Name of the joint (needed to load config from param server)
     */
    SimDrive(ros::NodeHandle& node_handle, const std::string& name);

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
     * @brief Callback function for publishing state and simulating drive
     *
     * @param event[in] Timer event
     */
    void callback_update(const ros::TimerEvent& event);

    /**
     * @brief Callback function which receives command for the drive
     *
     * @param target_state[in] Command
     */
    void callback_command(const DriveState::ConstPtr& target_state);


    struct timespec   _time_stamp_last_call;  //!< Saves the time stamp of the last call
  };

  class SimDriveManager : public Singleton<SimDriveManager>
  {
  friend class Singleton<SimDriveManager>;
  public:
    /**
     * @brief Initializes the drive manager
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
    SimDriveManager(void);

    std::vector<std::shared_ptr<SimDrive>>    _drive_list;     //!< List with simulated drives
  };
}



#endif /* SIM_DRIVES_HPP_ */
