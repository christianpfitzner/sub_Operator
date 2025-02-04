/*
 * hardware_control_loop.cpp
 *
 *  Created on: 01.12.2015
 *      Author: feesma44884
 */
#include <ohm_robotarm/moveit_interface_control_loop.h>

namespace ohm_robotarm
{
    MoveItControlLoop::MoveItControlLoop(ros::NodeHandle& node_handle,
         boost::shared_ptr<ohm_robotarm::MoveItControlInterface> hardware_interface) :
        _node_handle(node_handle),
        _moveit_interface(hardware_interface)
    {
        //get update frequency
        _node_handle.param("moveit_interface/update_rate", _update_rate, 100.0);

        ROS_DEBUG_STREAM_NAMED(OHM_ROBOTARM_APP_NAME, "Initializing hardware control manager. Update rate: " << _update_rate << " Hz");

        //create controller manager
        _controller_manager.reset(new controller_manager::ControllerManager(_moveit_interface.get(), _node_handle));

        //get current time
        clock_gettime(CLOCK_MONOTONIC, &_time_last);

        _loop   =   _node_handle.createTimer(ros::Duration(1.0 / _update_rate), &MoveItControlLoop::update, this);
    }

    void MoveItControlLoop::update(const ros::TimerEvent& e) {
        //check passed time
        clock_gettime(CLOCK_MONOTONIC, &_time_current);

        //calculate elapsed time
        _time_elapsed   =   ros::Duration(_time_current.tv_sec - _time_last.tv_sec + (_time_current.tv_nsec - _time_last.tv_nsec) / OHM_ROBOTARM_HCL_TIME_CONVERT);

        //update last time
        _time_last      =   _time_current;

        //read input data from hardware interface
        _moveit_interface->read(_time_elapsed);

        //update controller manager
        _controller_manager->update(ros::Time::now(), _time_elapsed);

        //write command
        _moveit_interface->write(_time_elapsed);
    }
};
