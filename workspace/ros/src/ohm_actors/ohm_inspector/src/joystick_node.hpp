/*
 * joystick.hpp
 *
 *  Created on: 24.11.2014
 *      Author: Martin Fees
 */

#ifndef INSPECTOR_JOYSTICK_HPP_
#define INSPECTOR_JOYSTICK_HPP_

#include <string>
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <ohm_actors_msgs/Command.h>

//#define USE_XBOX360
#define USE_XBOX360_AXES
//#define USE_PS3
//#define USE_XBOX360_HOME

/*
 * @JoyCommandConverter
 * @brief: This class converts the commands from the joysticks to the commands for the inspector
 */
class JoyCommandConverter
{
public:

    /*
     * Constructor
     */
    JoyCommandConverter();

    /*
     * Returns the command Data
     */
    void publishCommand();

private:

    /*
     * @joy: Joy state from joy_node
     *
     * Converts the data from the joystick to command data for the inspector
     */
    void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

    ros::NodeHandle                      _handle;           //handle of the ros node
    ros::Subscriber                      _joySubs;          //subscriber for the joystick
    ros::Publisher                       _inspectorPubl;    //publisher for inspector commands

    ohm_actors_msgs::Command               _data;             //data
};

#endif /* INSPECTOR_JOYSTICK_HPP_ */
