/*
 * ohm_tele_perspect_rect.cpp
 *
 *  Created on: 06.02.2019
 *      Author: volletjo
 */

#include <ros/ros.h>
// PCL specific includes
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud_conversion.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/transforms.h>
#include <Eigen/Dense>

ros::Publisher pub;

void cloud_cb (const sensor_msgs::PointCloud2ConstPtr& input)
{
  Eigen::Affine3f rotation180 = Eigen::Affine3f::Identity();
  float theta = M_PI;
  rotation180.rotate(Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitZ()));

  sensor_msgs::PointCloud2 input_cloud2;
  input_cloud2 = *input;
  pcl::PCLPointCloud2::Ptr input_pcl_cloud2(new pcl::PCLPointCloud2 ());
  pcl_conversions::toPCL(input_cloud2, *input_pcl_cloud2);
  pcl::PointCloud<pcl::PointXYZ> input_pcl_cloud;
  pcl::fromPCLPointCloud2(*input_pcl_cloud2, input_pcl_cloud);
  pcl::PointCloud<pcl::PointXYZ>::Ptr rotated_cloud (new pcl::PointCloud<pcl::PointXYZ> ());
  pcl::transformPointCloud(input_pcl_cloud, *rotated_cloud,rotation180);
  pcl::PCLPointCloud2 rotated_pcl_cloud2;
  pcl::toPCLPointCloud2(*rotated_cloud,rotated_pcl_cloud2);
  sensor_msgs::PointCloud2 rotated_cloud2;
  pcl_conversions::fromPCL(rotated_pcl_cloud2,rotated_cloud2);
  // Create a container for the data.

  // Do data processing here...
  //pcl_ros::transformPointCloud(*input_cloud, *rotated_cloud, rotation180);
  // Publish the data.
 pub.publish (rotated_cloud2);
}

int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "my_pcl_tutorial");
  ros::NodeHandle nh;

  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe ("/camera/depth/points", 1, cloud_cb);

  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("output", 1);

  // Spin
  ros::spin ();
}

