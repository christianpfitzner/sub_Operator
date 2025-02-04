/*
 * head_follow.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: phil
 */

#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_listener.h>

#include <string>

static ros::Publisher _pubTiltSpeed;
static ros::Publisher _pubPanSpeed;
static tf::TransformListener* _listener = NULL;
static std::string _tfChildFrame;
static geometry_msgs::PoseStamped* _transformed = NULL;
static double _timeout = 0.0;

static std_msgs::Float32 _anglePanCur;
static std_msgs::Float32 _angleTiltCur;

ros::Time _stampAnglePanCur;
ros::Time _stampAngleTiltCur;

void callBackTarget(const geometry_msgs::PoseStamped& target);
void callBackAngleTilt(const std_msgs::Float32& angle);
void callBackAnglePan(const std_msgs::Float32& angle);

void timerCallback(const ros::TimerEvent& e);


int main(int argc, char** argv)
{
  ros::init(argc, argv, "head_follow");
  ros::NodeHandle nh;
  ros::NodeHandle prvNh("~");

  std::string topicTiltSpeed;
  std::string topicPanSpeed;
  std::string topicTarget;
  double rate = 0.0;

  prvNh.param<std::string>("topic_tilt_speed", topicTiltSpeed, "/tilt/pos/des");
  prvNh.param<std::string>("topic_pan_speed", topicPanSpeed, "pan/pos/des");
  prvNh.param<std::string>("topic_target", topicTarget, "target");
  prvNh.param<std::string>("tf_child_frame", _tfChildFrame, "base_link");
  prvNh.param<double>("timeout", _timeout, 0.5);
  prvNh.param<double>("rate", rate, 20.0);

  _pubTiltSpeed = nh.advertise<std_msgs::Float32>(topicTiltSpeed, 1);
  _pubPanSpeed  = nh.advertise<std_msgs::Float32>(topicPanSpeed, 1);
  ros::Subscriber subsTarget = nh.subscribe(topicTarget, 1, callBackTarget);

  _listener = new tf::TransformListener;
  _transformed = new geometry_msgs::PoseStamped;

  ros::Timer timer = nh.createTimer(ros::Duration(1.0 / 20.0), timerCallback);

  _anglePanCur.data = 0.0;
  _angleTiltCur.data = 0.0;
  ros::spin();

  delete _listener;
}


void callBackTarget(const geometry_msgs::PoseStamped& target)
{
  try
  {
    _listener->transformPose(_tfChildFrame, target, *_transformed);
  }
  catch(tf::TransformException& ex)
  {
    std::cout << __PRETTY_FUNCTION__ << "Tf error " << ex.what() << std::endl;
    return;
  }
}

void timerCallback(const ros::TimerEvent& e)
{
//  geometry_msgs::PoseStamped target;
//  target.header.frame_id = "TCP_3";
//  target.header.stamp = ros::Time::now();
//  target.pose.orientation.w = 1.0;
//  if(!_listener->waitForTransform(target.header.frame_id, "TCP_3", target.header.stamp, ros::Duration(1.0)))
//    return;
  tf::StampedTransform tf;
  try
    {
      _listener->lookupTransform("sh_yaw_base", "arm_joint6_base", ros::Time(0), tf);
    }
    catch(tf::TransformException& ex)
    {
      std::cout << __PRETTY_FUNCTION__ << "Tf error " << ex.what() << std::endl;
      return;
    }

//  if((ros::Time::now() - _transformed->header.stamp).sec > _timeout)
//  {
//    std::cout << __PRETTY_FUNCTION__ << "error. Transformation outdated" << std::endl;
//    return;
//  }
//  if((ros::Time::now() - _stampAnglePanCur).sec > _timeout)
//  {
//    std::cout << __PRETTY_FUNCTION__ << "error. Pan angle outdated" << std::endl;
//    return;
//  }
//  if((ros::Time::now() - _stampAngleTiltCur).sec > _timeout)
//  {
//    std::cout << __PRETTY_FUNCTION__ << "error. Tilt angle outdated" << std::endl;
//    return;
//  }
  std_msgs::Float32 speedTiltDes;
  std_msgs::Float32 anglePanDes;
  const double angleTiltTCP = std::atan2(tf.getOrigin().getZ(), tf.getOrigin().getX());
  const double anglePanTCP = std::atan2(tf.getOrigin().getY(), tf.getOrigin().getX());
  //std::cout << __PRETTY_FUNCTION__ << " dz =  " << tf.getOrigin().getZ() << " dx = " << tf.getOrigin().getX() << std::endl;
  std::cout << __PRETTY_FUNCTION__ << " angle tilt tcp " << angleTiltTCP << std::endl;
  speedTiltDes.data = angleTiltTCP;//angleTiltTCP - _angleTiltCur.data;
  anglePanDes.data = anglePanTCP;



  _pubTiltSpeed.publish(speedTiltDes);
  _pubPanSpeed.publish(anglePanDes);
}

void callBackAngleTilt(const std_msgs::Float32& angle)
{
  _angleTiltCur = angle;
  _stampAngleTiltCur = ros::Time::now();
}

void callBackAnglePan(const std_msgs::Float32& angle)
{
  _anglePanCur = angle;
  _stampAnglePanCur = ros::Time::now();
}
