#include <iostream>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <std_msgs/Float64MultiArray.h>
#include <sensor_msgs/JointState.h>
//Replaced ohm_move with ohm_arm_interface
//#include <ohm_move/RobotarmCtrl.h>
#include <ohm_arm_interface/RobotarmCtrl.h>
#include <ohm_actors_msgs/DriveState.h>

#include "CyclicCoordinateDescent/CyclicCoordinateDescent.h"

ros::Subscriber _pose_sub;
//ros::Publisher _joint_pub;
ros::Publisher _js_pub;

ros::Publisher _joint_1_pub;
ros::Publisher _joint_2_pub;
ros::Publisher _joint_3_pub;
ros::Publisher _joint_4_pub;
ros::Publisher _joint_5_pub;
ros::Publisher _joint_6_pub;
ros::Publisher _joint_7_pub;
ros::Publisher _gripper_1_pub;
ros::Publisher _gripper_2_pub;

void callbackSetPose(const ohm_arm_interface::RobotarmCtrl& msg) {

  //constant variables for CCC Algorithm
  const double a[4] = { 0.0, 0.385, 0.458, 0.450 };
  const double alpha[4] = { M_PI / 2.0, 0.0, 0.0, 0.0 };
  const double d[4] = { 0, 0, 0, 0 };
  const double wo[3] = { 0, 0, 0 };
  const double alp = 1.0;
  const double Epsilon = 0.0000001;

  // subscribed vales of ohm_move node
  double Rd[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double pd[3] = { 0.0, 0.0, 0.0 };
  double theta_iv[7] = {0.0};

  //output variable with angles and number of iterations on the end
  double output_iv[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  for (size_t i = 0; i < 9; ++i) {
    Rd[i] = msg.drei_rot.data.at(i);
  }

  for (size_t j = 0; j < 3; ++j) {
    pd[j] = msg.des_pos.data.at(j);
  }

  for (size_t k = 0; k < 7; ++k) {
    theta_iv[k] = msg.theta_old.position.at(k);
  }

  double gr = msg.theta_old.position.at(7);

  // Call function to get the new angles
  CyclicCoordinateDescent(a, alpha, d, Rd, pd, theta_iv, wo, alp, Epsilon, output_iv);


  // round new angles which will be published
  for (size_t runden = 0; runden < 7; ++runden) {

    output_iv[runden] *= pow(10, 6);
    if (output_iv[runden] >= 0) {
      output_iv[runden] = floor(output_iv[runden] + 0.5);
    } else {
      output_iv[runden] = ceil(output_iv[runden] - 0.5);
    }
    output_iv[runden] /= pow(10, 6);

    if ((::fabs(output_iv[runden])) < 0.000000001) {
      output_iv[runden] = 0.0;
    }
  }

  const double q1 = output_iv[0];
  const double q2 = output_iv[1];
  const double q3 = output_iv[2];
  const double q4 = output_iv[3];
  const double q5 = output_iv[4];
  const double q6 = output_iv[5];
  const double q7 = output_iv[6];


//  std_msgs::Float64MultiArray joints;
//	joints.data.push_back(static_cast<float>(q1));
//	joints.data.push_back(static_cast<float>(q2));
//	joints.data.push_back(static_cast<float>(q3));
//	joints.data.push_back(static_cast<float>(q4));
//	joints.data.push_back(static_cast<float>(q5));
//	joints.data.push_back(static_cast<float>(q6));
//  joints.data.push_back(static_cast<float>(q7));
//	joints.data.push_back(static_cast<float>(*gr1));
//	joints.data.push_back(static_cast<float>(*gr2));
//  _joint_pub.publish(joints);

  // Publisher for Visualization and for Inverse Kinematics Calculation
  sensor_msgs::JointState js;
  js.header.stamp = ros::Time::now();
  js.header.frame_id = "";
  js.name.push_back("joint_1");
  js.name.push_back("joint_2");
  js.name.push_back("joint_3");
  js.name.push_back("joint_4");
  js.name.push_back("joint_5");
  js.name.push_back("joint_6");
  js.name.push_back("joint_7");
  js.name.push_back("joint_gripper_1");
  js.name.push_back("joint_gripper_2");

  js.position.push_back(q1);
  js.position.push_back(-q2);
  js.position.push_back(-q3);
  js.position.push_back(q4+M_PI); //gegen minus unendlich -q4 + M_PI
  js.position.push_back(q5);// -q5
  js.position.push_back(q6);
  js.position.push_back(q7); // -q7
  js.position.push_back(gr);
  js.position.push_back(-gr);

  _js_pub.publish(js);


  if (msg.amr_config == 0)
  {
  ROS_WARN("Press start to move the robot in real and not only in the simulation!");
	  return;
  }

  // The publishers below move the manipulator
  ohm_actors_msgs::DriveState j1;
  j1.header.stamp = ros::Time::now();
  j1.header.frame_id = "";
  j1.position = q1;
  _joint_1_pub.publish(j1);

  ohm_actors_msgs::DriveState j2;
  j2.header.stamp = ros::Time::now();
  j2.header.frame_id = "";
  j2.position = q2;
  _joint_2_pub.publish(j2);

  ohm_actors_msgs::DriveState j3;
  j3.header.stamp = ros::Time::now();
  j3.header.frame_id = "";
  j3.position = M_PI - q3;
  _joint_3_pub.publish(j3);

  ohm_actors_msgs::DriveState j4;
  j4.header.stamp = ros::Time::now();
  j4.header.frame_id = "";
  j4.position = -M_PI + q4;
  if (j4.position < 0.0) {
    j4.position = M_PI + q4;
  }
  _joint_4_pub.publish(j4);

  ohm_actors_msgs::DriveState j5;
  j5.header.stamp = ros::Time::now();
  j5.header.frame_id = "";
  j5.position = q5;
  _joint_5_pub.publish(j5);

  ohm_actors_msgs::DriveState j6;
  j6.header.stamp = ros::Time::now();
  j6.header.frame_id = "";
  j6.position = q6;
  _joint_6_pub.publish(j6);

  ohm_actors_msgs::DriveState j7;
  j7.header.stamp = ros::Time::now();
  j7.header.frame_id = "";
  j7.position = q7;
  _joint_7_pub.publish(j7);

  ohm_actors_msgs::DriveState g1;
  g1.header.stamp = ros::Time::now();
  g1.header.frame_id = "";
  g1.position = gr;
  _gripper_1_pub.publish(g1);

  ohm_actors_msgs::DriveState g2;
  g2.header.stamp = ros::Time::now();
  g2.header.frame_id = "";
  g2.position = -gr;
  _gripper_2_pub.publish(g2);
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "ohm_inverse_node");

  ros::NodeHandle nh;

  //_joint_pub = nh.advertise<std_msgs::Float64MultiArray>("joints", 1);
  _js_pub = nh.advertise<sensor_msgs::JointState>("/joint_visual", 1);

  _pose_sub = nh.subscribe("tcp/ctrl", 1, callbackSetPose);

  // Pulisher for the Joints and Gripper
  _joint_1_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_1/command", 1);
  _joint_2_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_2/command", 1);
  _joint_3_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_3/command", 1);
  _joint_4_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_4/command", 1);
  _joint_5_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_5/command", 1);
  _joint_6_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_6/command", 1);
  _joint_7_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_7/command", 1);
  _gripper_1_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_gripper_1/command", 1);
  _gripper_2_pub = nh.advertise<ohm_actors_msgs::DriveState>("joint_gripper_2/command", 1);

  ros::spin();

  return 0;
}
