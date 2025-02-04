/*
 * elmo_drives.hpp
 *
 *  Created on: 02.02.2016
 *      Author: feesma44884
 */

#ifndef ELMO_DRIVES_HPP_
#define ELMO_DRIVES_HPP_

#include <ros/ros.h>
#include <signal.h>

#include <ohm_can_interface/canopen.hpp>
#include <ohm_robotarm/drive.hpp>
#include <ohm_robotarm/singleton.hpp>
#include <std_srvs/Empty.h>

/***
 * @namespace ohm_robotarm
 *
 * @brief Namespace of the AutonOHM robotarm
 */
namespace ohm_robotarm
{
  /**
   * @class ElmoDrive
   *
   * @brief Class represents an drive controlled by an Elmo Motion Control MC
   */
  class ElmoDrive : public Drive::Drive
  {
  public:
    /**
     * @brief Constructor
     *
     * @param node_handle[in] ROS node handle
     * @param name[in] Name of the drive/joint
     * @param reset_drive[in] true will reset the drives (warning drive will be released!)
     * @param update_rate[in] Update rate of the drive in Hz
     */
    ElmoDrive(ros::NodeHandle& node_handle, const std::string& name, const bool reset_drive);

    /**
     * @brief Destructor
     */
    ~ElmoDrive(void);

    /**
     * @brief Initializes the Elmo drive
     *
     * @param node_handle[in] ROS node handle
     * @param update_rate[in] Update rate of the drive in Hz
     *
     * @return true if everything is ok
     */
    bool init(ros::NodeHandle& node_handle, const double update_rate);

    /**
     * @brief Releases all allocated memory and stops update timers
     */
    void release(void);

    /**
     * @brief Function called before operational mode
     *
     * @return true if everything is ok
     */
    bool pre_operational_config(void);

    void setResetDrives(const bool value){_reset_drive = value;}
    

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


    std::shared_ptr<CANopen::Node>  _node;           //!< Pointer to the CANopen node
    bool                      _reset_drive;    //!< If true drive will be reseted (warning drive will be released!)

    int64_t           _position_actual_value;        //!< Actual position of the drives in increments
    double            _position_target_old_value;    //!< Last target position

    int64_t           _velocity_actual_value;        //!< Actual velocity of the drives in increments/sec
    double            _velocity_target_old_value;    //!< Last target velocity

    bool              _new_command;                  //!< New command received

    bool              _is_released;           //!< Saves true if class is released

    
  };

  /**
   * @class ElmoDriveManager
   *
   * @brief Class manages the Elmo drives
   */
  class ElmoDriveManager : public Singleton<ElmoDriveManager>
  {
  friend class Singleton<ElmoDriveManager>;
  public:
    /**
     * @brief Initializes the Elmo Manager
     *
     * @param node_handle[in] ROS node handle
     * @param reset_drives[in] true will reset the drives (warning drive will be released!)
     *
     * @return true if everything is ok
     */
    bool init(ros::NodeHandle& node_handle, const bool reset_drives = false);

    /**
     * @brief Callback function called by CANopen Manager
     *
     * @return true if everything is ok
     */
    bool pre_operational_config(void);

    /**
     * @brief Returns the number of drives
     *
     * @return Number of drives
     */
    const size_t  get_num_drives(void);

    

  private:
    /**
     * @brief Constructor
     */
    ElmoDriveManager(void);
    bool callBackServiceReset(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res);
    std::vector<std::shared_ptr<ElmoDrive>>   _drive_list;      //!< List with elmo drives
    ros::ServiceServer _serverReset;
  };
};

#endif /* ELMO_DRIVES_HPP_ */
