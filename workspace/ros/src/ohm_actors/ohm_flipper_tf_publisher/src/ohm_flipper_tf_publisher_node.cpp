/*
 * ohm_flipper_tf_publisher_node.cpp
 *
 *  Created on: 24.04.2017
 *      Author: johannes
 */



#include <ros/ros.h>
#include <tf/transform_broadcaster.h>

#include <std_msgs/Float32.h>
#include <sensor_msgs/JointState.h>

ros::Subscriber _pose_sub;
ros::Subscriber _pose_sub1;
ros::Subscriber _pose_sub2;
ros::Subscriber _pose_sub3;

ros::Publisher _js_pub;



void callbackSetPose(const std_msgs::Float32& msg) {

   sensor_msgs::JointState js;

   js.header.stamp = ros::Time::now();
   js.header.frame_id = "";
   js.name.push_back("flipper_front_left_joint");
   js.position.push_back(msg.data);


   _js_pub.publish(js);

}

void callbackSetPose1(const std_msgs::Float32& msg) {

   sensor_msgs::JointState js;

   js.header.stamp = ros::Time::now();
   js.header.frame_id = "";
   js.name.push_back("flipper_front_right_joint");
   js.position.push_back(msg.data);


   _js_pub.publish(js);

}

void callbackSetPose2(const std_msgs::Float32& msg) {

   sensor_msgs::JointState js;

   js.header.stamp = ros::Time::now();
   js.header.frame_id = "";
   js.name.push_back("flipper_rear_left_joint");
   js.position.push_back(msg.data);


   _js_pub.publish(js);

}

void callbackSetPose3(const std_msgs::Float32& msg) {

   sensor_msgs::JointState js;

   js.header.stamp = ros::Time::now();
   js.header.frame_id = "";
   js.name.push_back("flipper_rear_right_joint");
   js.position.push_back(msg.data);


   _js_pub.publish(js);

}




int main(int argc, char** argv){
  ros::init(argc, argv, "flipper_publisher");

  ros::NodeHandle nh; //johannes



      _pose_sub = nh.subscribe( "flipper_front_left/angle/present",  1, callbackSetPose);
      _pose_sub1 = nh.subscribe("flipper_front_right/angle/present", 1, callbackSetPose1);
      _pose_sub2 = nh.subscribe("flipper_rear_left/angle/present",   1, callbackSetPose2);
      _pose_sub3 = nh.subscribe("flipper_rear_right/angle/present",  1, callbackSetPose3);

  _js_pub = nh.advertise<sensor_msgs::JointState>("flipper_states", 1);



  ros::spin();
  return 0;
};
