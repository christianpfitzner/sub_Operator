/*
 * ohm_visca_joy_control.cpp
 *
 *  Created on: 02.01.2019
 *      Author: volletjo
 */
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <ohm_visca_control/viscaControl.h>
#include <cmath>
#include "ViscaSend.h"

ros::Publisher viscaControl_pub;

int button_autofocus;
int button_zoom_tele;
int button_zoom_wide;
int button_focus_near;
int button_focus_far;
int axis_zoom;
int axis_focus;
int zoomSpeed_max;
int focusSpeed_max;
double deadzone_zoom;
double deadzone_focus;


void joystickCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  ohm_visca_control::viscaControl control_data;
  if(joy->buttons[button_autofocus] == 0)
  {
    control_data.autofocus = 0;
    control_data.teleStandard = joy->buttons[button_zoom_tele];
    control_data.wideStandard = joy->buttons[button_zoom_wide];
    control_data.variableSpeed = (uint8_t)(round(zoomSpeed_max*abs(static_cast<double>(joy->axes[axis_zoom]))));
    if((joy->axes[axis_zoom]) < -(deadzone_zoom))
    {
      control_data.wideVariable = 1;
    }
    else if((joy->axes[axis_zoom]) > deadzone_zoom)
    {
      control_data.teleVariable = 1;
    }
    else
    {
      control_data.teleVariable = 0;
      control_data.wideVariable = 0;
    }
  }

  if(joy->buttons[button_autofocus] == 1)
  {
    control_data.autofocus = 1;
    control_data.nearStandard = joy->buttons[button_focus_near];
    control_data.farStandard = joy->buttons[button_focus_far];
    control_data.variableLevel = (uint8_t)(round(focusSpeed_max*abs(static_cast<double>(joy->axes[axis_focus]))));
    if((joy->axes[axis_focus]) < -(deadzone_focus))
    {
      control_data.farVariable = 1;
    }
    else if((joy->axes[axis_focus]) > deadzone_focus)
    {
      control_data.nearVariable = 1;
    }
    else
    {
      control_data.farVariable = 0;
      control_data.nearVariable = 0;
    }
  }
  control_data.header.stamp = ros::Time::now();
  viscaControl_pub.publish(control_data);
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "visca_joy_control");
  ros::NodeHandle n;
  ros::NodeHandle _nh("~");
  ros::Subscriber joySub = n.subscribe("joy", 1, joystickCallback);
  viscaControl_pub = n.advertise<ohm_visca_control::viscaControl>("viscaControl", 1);
  _nh.param("button_autofocus", button_autofocus, 11);
  _nh.param("button_zoom_tele", button_zoom_tele, 13);
  _nh.param("button_zoom_wide", button_zoom_wide, 14);
  _nh.param("button_focus_near", button_focus_near, 15);
  _nh.param("button_focus_far", button_focus_far, 16);
  _nh.param("axis_zoom", axis_zoom, 1);
  _nh.param("axis_focus", axis_focus, 0);
  _nh.param("zoomSpeed_max", zoomSpeed_max, 7);
  _nh.param("focusSpeed_max", focusSpeed_max, 2);
  _nh.param("deadzone_zoom", deadzone_zoom, 0.01);
  _nh.param("deadzone_focus", deadzone_focus, 0.01);
  ros::spin();
return 0;
}
