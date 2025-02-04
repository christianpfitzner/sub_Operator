/*
 * test.cpp
 *
 *  Created on: 26.02.2016
 *      Author: feesma44884
 */
#include <ros/ros.h>
#include <ohm_actors_msgs/DriveState.h>
#include <ohm_robotarm/joint.hpp>

int main (int argc, char** argv) {
    //init ros system
    ros::init(argc, argv, "ohm_test");
    ros::NodeHandle node_handle;

    ohm_robotarm::Joint::Joint  new_joint(node_handle, "joint_1");

    ros::Publisher  joint_pub;
    joint_pub = node_handle.advertise<ohm_actors__msgs::DriveState>("/ohm_robotarm/joint_2/command", 10);

    ohm_robotarm::DriveState cmd;
    cmd.position  = -1.57;
    cmd.velocity  = 0;
    cmd.acceleration = 0;

    ros::Duration sleep(2.0);
    while(ros::ok()) {
      joint_pub.publish(cmd);
      ros::spinOnce();
      sleep.sleep();
    }
    return 0;
}
