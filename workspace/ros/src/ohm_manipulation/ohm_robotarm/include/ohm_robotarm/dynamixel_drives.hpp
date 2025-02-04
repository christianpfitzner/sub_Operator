/*
 * dynamixel_drives.hpp
 *
 *  Created on: 11.02.2016
 *      Author: feesma44884
 */

#ifndef DYNAMIXEL_DRIVES_HPP_
#define DYNAMIXEL_DRIVES_HPP_

#include <ros/ros.h>
#include <signal.h>

#include <ohm_robotarm/drive.hpp>
#include <ohm_robotarm/serial_rs232.hpp>
#include <ohm_robotarm/singleton.hpp>

/***
 * @namespace ohm_robotarm
 *
 * @brief Namespace of the AutonOHM robotarm
 */
namespace ohm_robotarm
{
  /**
   *  @class DynamixelDrive
   *
   *  @brief Class representing a dynamixel drive
   */
  class DynamixelDrive : public Drive::Drive
  {
  public:
    /**
     * @brief Constructor
     *
     * @param node_handle[in] Reference to the node handle
     * @param name[in] Name of the drive (needed to load config from param server)
     */
    DynamixelDrive(ros::NodeHandle node_handle, const std::string& name);

    /**
     * @brief Destructor
     */
    ~DynamixelDrive(void);

    /**
     * @brief Initializes the Dynamixel drive
     *
     * @param node_handle[in] Reference to the node handle
     * @param update_rate[in] Update rate of the drive in Hz
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

    /**
     * @brief Updates the actual state of the dynamixel drive
     */
    void update_state(void);

    /**
     * @brief Sends command to the drive
     */
    void send_commands(void);


    int16_t   _position_actual;    //!< Actual position of the drive
    double    _position_old;       //!< Old position of the drive
    bool      _new_command;        //!< Saves true if new command was received
  };

  /**
   * @class DynamixelDriveManager
   *
   * @brief This class manages all dynamixel drives of the robotarm
   */
  class DynamixelDriveManager : public Singleton<DynamixelDriveManager>
  {
  friend class Singleton<DynamixelDriveManager>;
  public:
    /**
     * @brief Initializes the dynamixel drive manager
     *
     * @param node_handle[in] ROS node handle
     * @param serial_port_path[in] Path to the serial port
     * @return
     */
    bool init(ros::NodeHandle& node_handle, const std::string serial_port_path);

    /**
     * @brief Returns the number of drives
     *
     * @return Number of drives
     */
    const size_t  get_num_drives(void);

    /**
     * @brief Returns instance of virtual com port
     * @return
     */
    RS232::Port&  get_VCP(void) {return _vcp;}

  private:
    /**
     * @brief Constructor
     */
    DynamixelDriveManager(void);

    RS232::Port                                     _vcp;                   //!< Virtual COM Port
    std::vector<std::shared_ptr<DynamixelDrive>>    _dynamixel_drive_list;  //!< List with dynamixel drives
  };
};


#endif /* DYNAMIXEL_DRIVES_HPP_ */
