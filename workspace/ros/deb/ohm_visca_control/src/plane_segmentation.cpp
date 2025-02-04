/*
 * plane_segmentation.cpp
 *
 *  Created on: 19.03.2019
 *      Author: volletjo
 */
#include <ros/ros.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <pcl_ros/point_cloud.h>
#include <math.h>
#include <std_msgs/Float32.h>
#include <depth_image_proc/depth_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud_conversion.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/transforms.h>
#include <pcl/sample_consensus/method_types.h>
//#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/ModelCoefficients.h>
#include <Eigen/Dense>
#include <pcl_ros/point_cloud.h>
#include <pcl/filters/passthrough.h>


//ros::Publisher _pub_plane_orientation;
ros::Publisher _pub_pass_filtered_cloud;
float _average_distance;

void average_dist_CB(const std_msgs::Float32::ConstPtr& msg)
{
  _average_distance = (msg->data) / 1000.0;
}

void cloud_CB(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr& input_cloud)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PassThrough<pcl::PointXYZ> pass_filter;
  pass_filter.setInputCloud (input_cloud);
  pass_filter.setFilterFieldName ("z");
  pass_filter.setFilterLimits ((1),(4) );
  pass_filter.filter (*cloud_filtered);
  cloud_filtered->header.frame_id = "camera_link";
  //cloud_filtered->height = input_cloud->height;
  //cloud_filtered->width = input_cloud->width;
  pcl_conversions::toPCL(ros::Time::now(), cloud_filtered->header.stamp);
  _pub_pass_filtered_cloud.publish(*cloud_filtered);
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "plane_segmentation");
  ros::NodeHandle n;
  ros::Subscriber average_distance_sub = n.subscribe("med_dist_to_plane", 1, average_dist_CB);
  ros::Subscriber cloud_sub = n.subscribe("/camera/depth/points", 1, cloud_CB);
  _pub_pass_filtered_cloud = n.advertise<pcl::PointCloud<pcl::PointXYZ>> ("pass_filtered_cloud", 1);
  ros::spin();
  return 0;
}
