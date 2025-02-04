/*
 * mark_victim.cpp
 *
 *  Created on: May 5, 2019
 *      Author: phil
 */


#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <std_srvs/Empty.h>
#include <tf/transform_listener.h>
#include <laser_geometry/laser_geometry.h>
#include <sensor_msgs/PointCloud.h>
#include <ohm_perception_msgs/Victim.h>

void callBackScan(const sensor_msgs::LaserScan& scan);
bool callBackMark(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res);


static sensor_msgs::LaserScan _scan;
static tf::TransformListener* _listener;
static laser_geometry::LaserProjection _projector;
static sensor_msgs::PointCloud _cloud;
static ros::Publisher _pubVictim;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "mark_victim");
  ros::NodeHandle _nh;
  _listener = new tf::TransformListener;
 _pubVictim = _nh.advertise<ohm_perception_msgs::Victim>("/victim/valid", 1);
 ros::ServiceServer srv = _nh.advertiseService("/mark", callBackMark);
 ros::Subscriber subsScan = _nh.subscribe("scan", 1, callBackScan);
 ros::spin();
}

void callBackScan(const sensor_msgs::LaserScan& scan)
{
  //std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
  _cloud.points.clear();
  if(!_listener->waitForTransform(scan.header.frame_id,
          "/map",
          scan.header.stamp + ros::Duration().fromSec(scan.ranges.size()*scan.time_increment),
          ros::Duration(1.0)))
  {
    std::cout << __PRETTY_FUNCTION__ << "error waiting for transform" << std::endl;
       return;
  }

  _projector.transformLaserScanToPointCloud("/map", scan, _cloud, *_listener);
}

bool callBackMark(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res)
 {
  //std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
  static int id = 0;
  if(!_cloud.points.size())
  {
    std::cout << __PRETTY_FUNCTION__ << "error cloud empty" << std::endl;
    return false;
  }
  ohm_perception_msgs::Victim vic;
  vic.id = id++;
  vic.valid = true;
  vic.pose.position.x = _cloud.points[_cloud.points.size() / 2].x;
  vic.pose.position.y = _cloud.points[_cloud.points.size() / 2].y;
  vic.pose.position.z = 0.0;
  _pubVictim.publish(vic);

//  int32 NONE=-1
//  int32 id
//  int32 serial
//  int32 u
//  int32 v
//  int32 width
//  int32 height
//  bool valid
//  bool checked
//  geometry_msgs/Pose pose
  return true;
}
