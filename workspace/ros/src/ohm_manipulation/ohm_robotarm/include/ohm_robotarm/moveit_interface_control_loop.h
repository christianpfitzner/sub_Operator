/*
 * moveit_interface_control_loop.h
 *
 *  Created on: 01.12.2015
 *      Author: feesma44884
 */

#ifndef MOVEIT_INTERFACE_CONTROL_LOOP_H_
#define MOVEIT_INTERFACE_CONTROL_LOOP_H_

#include <ohm_robotarm/moveit_control_interface.h>
#include <ros/ros.h>
#include <time.h>

#define OHM_ROBOTARM_HCL_TIME_CONVERT    1000000000.0

/**
 * @namespace ohm_robotarm
 *
 * @brief namespace of ohm manipulator
 */
namespace ohm_robotarm
{

/**
 * @class MoveItControlLoop
 *
 * @brief Control loop of the MoveIt! Interface
 */
class MoveItControlLoop
{
public:

    /**
     * @brief Constructor of hardware control loop
     *
     * @param node_handle[in] ROS node handle
     * @param moveit_interface[in] Interface between moveit and the robotarm drives
     */
    MoveItControlLoop(ros::NodeHandle& node_handle, boost::shared_ptr<ohm_robotarm::MoveItControlInterface> moveit_interface);

    /**
     * @brief Updates the control of ohm manipulator
     */
    void update(const ros::TimerEvent& e);

protected:

    /**
     * ROS node handle
     */
    ros::NodeHandle _node_handle;    /***< ROS node handle */

    /**
     * Variable for calculating correct timing
     */
    double              _update_rate;   /***< Update frequency of the control loop */
    ros::Duration       _time_elapsed;  /***< Elapsed time */
    struct timespec     _time_last;     /***< Timestamp of last call */
    struct timespec     _time_current;  /***< Current timestamp */
    ros::Timer          _loop;          /***< Loop for ros control update */

    /**
     * Controller manager and hardware interface
     */
    boost::shared_ptr<controller_manager::ControllerManager>    _controller_manager; /***< This class manages load, unloading, starting and stopping ros_control based controller*/
    boost::shared_ptr<ohm_robotarm::MoveItControlInterface>     _moveit_interface;   /***< MoveIt! interface to ohm manipulator hardware */
};

};

#endif /* MOVEIT_INTERFACE_CONTROL_LOOP_H_ */
