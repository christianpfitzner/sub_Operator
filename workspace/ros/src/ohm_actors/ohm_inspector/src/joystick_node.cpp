/*
 * joystick.cpp
 *
 *  Created on: 24.11.2014
 *      Author: Martin Fees
 *
 */

#include "joystick_node.hpp"

JoyCommandConverter::JoyCommandConverter() {
    //subscribe messages from joystick
    _joySubs       = _handle.subscribe<sensor_msgs::Joy>("joy", 10, &JoyCommandConverter::joyCallback, this);

    //publisher
    _inspectorPubl = _handle.advertise<ohm_actors_msgs::Command>("inspector_cmd", 10);
}

void JoyCommandConverter::publishCommand() {
    _inspectorPubl.publish(_data);
}

void JoyCommandConverter::joyCallback(const sensor_msgs::Joy::ConstPtr& joy) {

    float   axes[4] =   {joy->axes[0], joy->axes[1], joy->axes[4], joy->axes[3]};

    //set data
    for(int i = 0; i < 4; i++) {
        if(fabsf(axes[i]) < 0.2f) {
            axes[i] =   0.0f;
        }
    }

    //set speeds
    _data.speed2D[0]    =   -axes[0];
    _data.speed2D[1]    =   axes[1];
    _data.speedAxis[0]  =   -axes[3];
    _data.speedAxis[1]  =   axes[2];

    //check button commands
    _data.moveHome = joy->buttons[4];
    _data.movePark = joy->buttons[6];

    _data.speedTCP = -(joy->axes[13]) + (joy->axes[12]);

}

int main(int argc, char** argv) {
    //init ros system
    ros::init(argc, argv, "joy_to_manipulator");

    //start converter
    JoyCommandConverter converter;

    ros::Rate    loopRate(30.0);
    while(ros::ok()) {

        converter.publishCommand();

        ros::spinOnce();
        loopRate.sleep();
    }

    return 0;
}
