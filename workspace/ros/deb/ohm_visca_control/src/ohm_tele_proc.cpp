/*
 * ohm_tele_proc.cpp
 *
 *  Created on: 24.01.2019
 *      Author: volletjo
 */

#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <math.h>
#include "std_msgs/UInt16.h"
#include <std_msgs/Float32.h>
#include <depth_image_proc/depth_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud_conversion.h>
#include <geometry_msgs/Point.h>
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
//#include <pcl/filters/voxel_grid.h>
//#include <pcl_ros/point_cloud.h>
//#include <pcl/visualization/cloud_viewer.h>
//#include <pcl/sample_consensus/ransac.h>


image_transport::Publisher _pub_tele_rect;
image_transport::Publisher _pub_tele_rect_cropped;
image_transport::Publisher _pub_overview;
image_transport::Publisher _pub_cropped_depth_image;
//image_transport::Publisher _pub_ir255;
ros::Publisher _pub_cropped_pointcloud;
ros::Publisher _pub_plane_pointcloud;
ros::Publisher _pub_med_dist_to_plane;
ros::Publisher _pub_projected_tele_center;

ros::Subscriber sub_rgbd_rgb_info;
ros::Subscriber sub_rgbd_depth_info;

int tele_resX = 1280;
int tele_resY = 720;
int orbbec_depth_resX = 100;
int orbbec_depth_resY = 200;
int orbbec_rgb_resX = 640;
int orbbec_rgb_resY = 480;
//TODO nochmal mit dem nicht rektifizierten messen
double orbbec_angleX =59.035;
double orbbec_angleY =46.996;
bool depthImageAvailable =1;
//double camera_horizontal_degree_tele = 6.55;
//double camera_horizontal_degree_wide = 49;
//double factor_horizontal_to_vertical = 0.563;
//
//double factor_degree_zoom_1 = -2.5691e-3;
//double factor_degree_zoom_0 = 49.0;

//FoV factors horizontal for rect image
double x3H = 5.0e-12;
double x2H = -1.0e-8;
double x1H = -4.3e-3;
double x0H = 59.034;

//FoV factors vertical for rect image
double x3V = 2.0e-12;
double x2V = 2.0e-8;
double x1V = -2.8e-3;
double x0V = 35.183;

//FOV potential factors
double x1Vpot = 37.1631023296634;
double expVpot = -0.97298709152989;
double x1Hpot = 64.3200937833562;
double expHpot = -0.994261410323114;

cv::Point2f center;
float centerOffsetX = 0;
float centerOffsetY = 0;
double camera_distance_mm = 21.40;

cv::Point ul_tele;
cv::Point lr_tele;

//Zoom Factors
double z_3 = 0.00000000000276866;
double z_2 = -0.00000002398916892;
double z_1 = 0.00019946657154348;
double z_0 = 0.99960174706693600;

double camera_zoom_times_rec = 0;
double camera_zoom_times = 0;
double camera_zoom_value = 0;

////Camera Matrix Factors
double cx = 640.0;
double cy = 360.0;

double f_2 = 5.48070e+1;
double f_1 = 7.95708e+2;
double f_0 = 2.79870e+2;

double k1_4 = 3.73573e-3;
double k1_3 = -3.03673e-2;
double k1_2 = 2.58195e-1;
double k1_1 = -6.39061e-2;
double k1_0 = -4.03481e-1;

double k2_5 = -8.51668e-2;
double k2_4 = 1.45158e+0;
double k2_3 = -8.81619e+0;
double k2_2 = 2.14551e+1;
double k2_1 = -2.30282e+1;
double k2_0 = 9.01427e+0;

double t1_4 = -2.13956e-5;
double t1_3 = 4.93715e-4;
double t1_2 = -3.91295e-3;
double t1_1 = 1.38647e-2;
double t1_0 = -1.49924e-2;

double t2_4 = -1.01183e-4;
double t2_3 = 1.86619e-3;
double t2_2 = -1.03982e-2;
double t2_1 = 2.29370e-2;
double t2_0 = -1.62786e-2;

double average_distance = 0;
double last_average_distance = 0;
double last_pixel_offset = 0;

double calib_factor = 1.35;
//double calib_factor = 1.0;
double viewable_angle_horizontal_16384 = calib_factor*(x3H*pow(camera_zoom_value, 3.0) + x2H*pow(camera_zoom_value, 2.0) + x1H*camera_zoom_value + x0H);
double viewable_angle_vertical_16384 = calib_factor*(x3V*pow(camera_zoom_value, 3.0) + x2V*pow(camera_zoom_value, 2.0) + x1V*camera_zoom_value + x0V);
double linear_horizontal_16384_x1 = -0.00177;
double linear_horizontal_16384_x0 = + 34.704;

Eigen::Matrix3f intrinsic;

//intrinsic << 570.3405151367188, 0.0, 314.5, 0.0, 570.3405151367188, 235.5, 0.0, 0.0, 1.0;
double depth_camera_cx = 314.5;
double depth_camera_cy = 235.5;
double depth_camera_fx = 570.3405151367188;
double depth_camera_fy = 570.3405151367188;

uint16_t last_zoom_value;

//pcl::PointCloud<pcl::PointXYZ>::Ptr DepthImagetoPCL(cv::Mat depth_image, Eigen::Matrix3f& _intrinsics)
//{
//  pcl::PointCloud<pcl::PointXYZ>::Ptr pointcloud(new pcl::PointCloud<pcl::PointXYZ>);
//
//    float fx = _intrinsics(0, 0);
//    float fy = _intrinsics(1, 1);
//    float cx = _intrinsics(0, 2);
//    float cy = _intrinsics(1, 2);
//
//    float factor = 1;
//
//    depth_image.convertTo(depth_image, CV_32F); // convert the image data to float type
//
//    if (!depth_image.data) {
//        std::cerr << "No depth data!!!" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    pointcloud->width = depth_image.cols; //Dimensions must be initialized to use 2-D indexing
//    pointcloud->height = depth_image.rows;
//    pointcloud->resize(pointcloud->width*pointcloud->height);
//
////#pragma omp parallel for
//    for (int v = 0; v < depth_image.rows; v++)
//    {
//        for (int u = 0; u < depth_image.cols; u++)
//        {
//            float Z = depth_image.at<float>(v, u) / factor;
//
//            pcl::PointXYZ p;
//            p.z = Z;
//            p.x = (u - cx) * Z / fx;
//            p.y = (v - cy) * Z / fy;
//
////            p.z = p.z / 1000;
////            p.x = p.x / 1000;
////            p.y = p.y / 1000;
//
//            pointcloud->points.push_back(p);
//
//        }
//    }
//
//    return pointcloud;
//
//}
void rgb_info_CB(const sensor_msgs::CameraInfo& rgb_camera_info)
{
  orbbec_rgb_resX = rgb_camera_info.width;
  orbbec_rgb_resY = rgb_camera_info.height;
  center.x = rgb_camera_info.K[2] + centerOffsetX;
  center.y = rgb_camera_info.K[5] + centerOffsetY;
  sub_rgbd_rgb_info.shutdown();
//  std::cout<< "rgb: "<<orbbec_rgb_resX <<", "<< orbbec_rgb_resY << std::endl;
}

void depth_info_CB(const sensor_msgs::CameraInfo& depth_camera_info)
{
  orbbec_depth_resX = depth_camera_info.width;
  orbbec_depth_resY = depth_camera_info.height;
  depth_camera_cx = depth_camera_info.K[2];
  depth_camera_cy = depth_camera_info.K[5];
  depth_camera_fx = depth_camera_info.K[0];
  depth_camera_fy = depth_camera_info.K[4];
//  std::cout << "1: "<<"orbbec_depth_cx: " << depth_camera_cx << ", orbbec_depth_cy: " << depth_camera_cy << std::endl;
  sub_rgbd_depth_info.shutdown();
//  std::cout<< "depth: "<<orbbec_depth_resX <<", "<< orbbec_depth_resY << std::endl;
//  std::cout<< "K: "<<depth_camera_cx <<", "<< depth_camera_cy << ", " << depth_camera_fx << ", " << depth_camera_fy << std::endl;
}

void camera_zoom_CB(const std_msgs::UInt16::ConstPtr& camera_zoom)
{
  camera_zoom_value = camera_zoom->data;
  double camera_zoom_rec_value = 0;
  if(camera_zoom_value > 31424)
  {
    camera_zoom_value = last_zoom_value;
  }
  else
  {
    last_zoom_value = camera_zoom_value;
  }
  if(camera_zoom_value >= 16384)
  {
    camera_zoom_rec_value = 16384;
  }
  else
  {
    camera_zoom_rec_value = camera_zoom_value;
  }
  camera_zoom_times_rec =(z_3*pow(static_cast<double>(camera_zoom_rec_value), 3.0))+(z_2*pow(static_cast<double>(camera_zoom_rec_value), 2.0))+(z_1*static_cast<double>(camera_zoom_rec_value))+z_0;
  camera_zoom_times =(z_3*pow(static_cast<double>(camera_zoom_value), 3.0))+(z_2*pow(static_cast<double>(camera_zoom_value), 2.0))+(z_1*static_cast<double>(camera_zoom_value))+z_0;
//  std::cout << "ZOOM TIMES REC: x" <<  camera_zoom_times_rec << std::endl;
//  std::cout << "ZOOM TIMES : x" <<  camera_zoom_times << std::endl;
}

void TeleImageCallback(const sensor_msgs::ImageConstPtr& img)
{
  cv_bridge::CvImagePtr inMsgPtr;
  cv_bridge::CvImagePtr outImgPtr(new cv_bridge::CvImage);
  inMsgPtr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
  cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32FC1);
  cv::Mat distortion(1, 5, CV_32FC1);
//  std::cout<< camera_zoom_value << std::endl;
//  cv::Point2f center;
//  center.x = 640.0;
//  center.y = 360.0;
//  cv::Point ul;
//  ul.x = center.x - 2;
//  ul.y = center.y + 2;
//  cv::Point lr;
//  lr.x = center.x + 2;
//  lr.y = center.y -2;
//  cv::Scalar color;
//  cv::rectangle(inMsgPtr->image,ul,lr,cv::viz::Color(0,0,255),CV_FILLED);
//  cv::imshow( "image tele undistort", inMsgPtr->image);
  //  cv::waitKey(25);
//  std::cout<< camera_zoom_times << std::endl;
  float f = static_cast<float>((f_2*pow(camera_zoom_times_rec, 2.0))+(f_1*camera_zoom_times_rec)+f_0);
          cameraMatrix.at<float>(0,0) = f;
          cameraMatrix.at<float>(0,1) = 0.0;
          cameraMatrix.at<float>(0,2) = cx;
          cameraMatrix.at<float>(1,0) = 0.0;
          cameraMatrix.at<float>(1,1) = f;
          cameraMatrix.at<float>(1,2) = cy;
          cameraMatrix.at<float>(2,0) = 0.0;
          cameraMatrix.at<float>(2,1) = 0.0;
          cameraMatrix.at<float>(2,2) = 1.0;

                distortion.at<float>(0,0) = static_cast<float>((k1_4*pow(camera_zoom_times_rec, 4.0))+(k1_3*pow(camera_zoom_times_rec, 3.0))+(k1_2*pow(camera_zoom_times_rec, 2.0))+(k1_1*camera_zoom_times_rec)+k1_0);
                distortion.at<float>(0,1) = static_cast<float>((k2_5*pow(camera_zoom_times_rec, 5.0))+(k2_4*pow(camera_zoom_times_rec, 4.0))+(k2_3*pow(camera_zoom_times_rec, 3.0))+(k2_2*pow(camera_zoom_times_rec, 2.0))+(k2_1*camera_zoom_times_rec)+k2_0);
                distortion.at<float>(0,2) = static_cast<float>((t1_4*pow(camera_zoom_times_rec, 4.0))+(t1_3*pow(camera_zoom_times_rec, 3.0))+(t1_2*pow(camera_zoom_times_rec, 2.0))+(t1_1*camera_zoom_times_rec)+t1_0);
                distortion.at<float>(0,3) = static_cast<float>((t2_4*pow(camera_zoom_times_rec, 4.0))+(t2_3*pow(camera_zoom_times_rec, 3.0))+(t2_2*pow(camera_zoom_times_rec, 2.0))+(t2_1*camera_zoom_times_rec)+t2_0 );
                distortion.at<float>(0,4) = 0.0;

 //std::cout << cameraMatrix << std::endl;
//  std::cout << distortion << std::endl;
  cv::Mat undistortImage;
  cv::undistort(inMsgPtr->image,undistortImage,cameraMatrix,distortion,cameraMatrix);
  cv::Size undistortSize = undistortImage.size();
  uint16_t undistort_rows = undistortSize.height;
  uint16_t undistort_cols = undistortSize.width;
  uint16_t x1 = 0, x2 = 0, y1 = 0, y2 = 0;
  uint8_t colorTh = 3;

  for (int i=0; i < undistort_cols/2; i++)
  {
    cv::Vec3b color = undistortImage.at<cv::Vec3b>(cv::Point(i, undistort_rows/2));
    if(x1 == 0 && (color.val[0] >= colorTh || color.val[1] >= colorTh || color.val[2] >= colorTh))
    {
      x1 = i;
      break;
    }
  }
  for (int i=undistort_cols - 1; i > undistort_cols/2; i--)
  {
    cv::Vec3b color = undistortImage.at<cv::Vec3b>(cv::Point(i, undistort_rows/2));
    if(x2 == 0 && (color.val[0] >= colorTh || color.val[1] >= colorTh || color.val[2] >= colorTh))
    {
      x2 = i + 1;
      break;
    }
  }
  for (int i=0; i < undistort_rows/2; i++)
  {
    cv::Vec3b color = undistortImage.at<cv::Vec3b>(cv::Point(undistort_cols/2, i));
    if(y1 == 0 && (color.val[0] >= colorTh || color.val[1] >= colorTh || color.val[2] >= colorTh))
    {
      y1 = i;
      break;
    }
  }
  for (int i = undistort_rows - 1; i > undistort_rows/2; i--)
  {
    cv::Vec3b color = undistortImage.at<cv::Vec3b>(cv::Point(undistort_cols/2, i));
    if(y2 == 0 && (color.val[0] >= colorTh || color.val[1] >= colorTh || color.val[2] >= colorTh))
    {
      y2 = i + 1;
      //std::cout  << (uint8_t)(color.val[0]) << ", " << (uint8_t)(color.val[1]) << ", " << (uint8_t)(color.val[2]) << std::endl;
      break;
    }
  }
  //std::cout << "x1: " << x1 << ", x2: " << x2 << ", y1: " << y1 << ", y2: " << y2 << std::endl;
  cv::Rect roi;
  roi.x = x1;
  roi.y = y1;
  roi.width = x2 - x1;
  roi.height = y2 -y1;
  cv::Mat croppedImage = undistortImage(roi);
//  std::cout << "rows: " << undistort_rows << ", cols: " << undistort_cols << std::endl;
//  cv::rectangle(output_image,ul,lr,cv::viz::Color(0,0,255),CV_FILLED);
  ros::Time time = ros::Time::now();
  outImgPtr->encoding = "bgr8";
  outImgPtr->header.stamp = time;
  outImgPtr->header.frame_id = "/tele_rect";
  outImgPtr->image = undistortImage;
  _pub_tele_rect.publish(outImgPtr->toImageMsg());

  outImgPtr->encoding = "bgr8";
  outImgPtr->header.stamp = time;
  outImgPtr->header.frame_id = "/tele_rect_cropped";
  outImgPtr->image = croppedImage;
  _pub_tele_rect_cropped.publish(outImgPtr->toImageMsg());

}

void DepthImageCallback(const sensor_msgs::ImageConstPtr& img)
{

//  std::cout << "-----------------------------------"<<std::endl;
  cv::Mat input_depth_image;
  cv_bridge::CvImagePtr inImgPtr;
  cv_bridge::CvImagePtr outImgPtr(new cv_bridge::CvImage);
  inImgPtr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::TYPE_16UC1);
  cv::Mat input_depth_float;
  input_depth_image = inImgPtr->image;
  input_depth_image.convertTo(input_depth_float,CV_32FC1);
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5),cv::Point(-1,-1));
  cv::dilate(input_depth_float,input_depth_float, element, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);
//  cv::GaussianBlur(input_depth_float,input_depth_float,cv::Size(3,3), 0.75);
//  cv::Point2f center;
//  center.x = orbbec_depth_resX/2;
//  center.y = orbbec_depth_resY/2;
//  std::cout << "4: center.y" << center.y << std::endl;
  cv::Size crop;
  crop.height = 6;
  crop.width = 6;
  cv::Mat rotation = getRotationMatrix2D(center, 0.0, 1.0);
  //cv::Mat rotated_img;
  cv::Size image_size;
  image_size.width = orbbec_depth_resX;
  image_size.height = orbbec_depth_resY;
  //std::cout<< "orbbec_X: " <<orbbec_depth_resX<< ", orbbec_Y: " <<orbbec_depth_resY<< std::endl;
  //std::cout<< "Hier1 "<<std::endl;
  //cv::warpAffine(input_depth_float, rotated_img, rotation, image_size, cv::INTER_LINEAR, cv::BORDER_DEFAULT);
  cv::Mat cropped_img;
  //cv::getRectSubPix(rotated_img, crop, center,cropped_img);
  cv::getRectSubPix(input_depth_float, crop, center,cropped_img);
//  cv::imshow("whatthe", cropped_img);
  cropped_img.convertTo(outImgPtr->image,CV_16UC1);
  cv::Scalar average_dist_scal = cv::mean(cropped_img);
  average_distance = average_dist_scal[0];
  if (average_distance <= 200)
  {
    average_distance = last_average_distance;
  }
  else
  {
    last_average_distance = average_distance;
  }
//  std::cout<< "average_distance: " <<average_distance<< std::endl;
  std_msgs::Float32 aver_dist_msg;
  aver_dist_msg.data = static_cast<float>(average_distance);
  _pub_med_dist_to_plane.publish(aver_dist_msg);
cv::Size tele_crop;
tele_crop.height = ul_tele.y-lr_tele.y;
tele_crop.width = lr_tele.x-ul_tele.x;
//std::cout<<"cropped_width: " << tele_crop.width<<std::endl;
//std::cout<<"cropped_height: " << tele_crop.height<<std::endl;
  if ((tele_crop.height > 0) && (tele_crop.width > 0))
  {
cv::Point2f center_tele;
center_tele.x = ul_tele.x+((lr_tele.x-ul_tele.x)/2);
center_tele.y = ul_tele.y+((lr_tele.y-ul_tele.y)/2);
//cv::rectangle(rotated_img,ul_tele,lr_tele,cv::viz::Color(0,0,255),1);

cv::Mat tele_cropped_img;
//cv::getRectSubPix(rotated_img, tele_crop, center_tele,tele_cropped_img);
//cv::getRectSubPix(input_depth_float, tele_crop, center_tele,tele_cropped_img, -1);
cv::Rect roi;
//std::cout << "ul: " << ul_tele.x << ", lr : " << lr_tele.y << ", height: " << tele_crop.height << ", width: "<< tele_crop.width << std::endl;
int roi_rows = 0;
int roi_cols = 0;
//if(ul_tele.x <= 0)
//{
//  roi.x=0;
//  roi_cols += (int)ul_tele.x;
//}
//else
//{
//  roi.x=(int)ul_tele.x;
//}
//if(lr_tele.y <= 0)
//{
//  roi.y=0;
//  roi_rows += (int)lr_tele.y;
//}
//else
//{
//  roi.y=lr_tele.y;
//}
//if ((lr_tele.y+tele_crop.height)>= input_depth_float.rows)
//{
//  roi.height = roi_rows - (int)lr_tele.y;
//}
//else
//{
//  roi.height =tele_crop.height;
//}
//if ((ul_tele.x+tele_crop.width)>=input_depth_float.cols)
//{
//  roi.width = roi_cols - (int)ul_tele.x;
//}
//else
//{
//  roi.width = tele_crop.width;
//}
int roi_tele_x = center_tele.x - tele_crop.width/2;
int roi_tele_y = center_tele.y - tele_crop.height/2;
//std::cout << "3: roi_tele_y: " << roi_tele_y << ", center_tele.y: " << center_tele.y << ", tele_crop.height: " << tele_crop.height << std::endl;
if(roi_tele_x <= 0)
{
  roi.x = 0;
  roi.width = input_depth_image.cols;// - abs(roi_tele_x);
  depthImageAvailable = 0;
  //std::cout << "HIER1 " << roi.width << "; roi_x :"<<roi_tele_x << std::endl;
}
else if((roi_tele_x+tele_crop.width) >=input_depth_float.cols)
{
  roi.x = roi_tele_x;
  roi.width = input_depth_float.cols - (abs(roi_tele_x));
  depthImageAvailable = 0;
  //std::cout << "HIER2 " << roi.width<< "; roi_x :"<<roi_tele_x << std::endl;
}
else
{
  roi.x = roi_tele_x;
  roi.width = tele_crop.width;
  depthImageAvailable = 1;
  //std::cout << "HIER3 "<< tele_crop.width<< std::endl;
}

if(roi_tele_y <= 0)
{
  roi.y = 0;
  roi.height=input_depth_image.rows;//tele_crop.height - abs(roi_tele_y);
  depthImageAvailable = 0;
//  std::cout << "HIER1 " << roi.height << "; roi_y :"<<roi_tele_y << std::endl;
}
else if ((roi_tele_y+tele_crop.height) >= input_depth_float.rows)
{
  roi.y = roi_tele_y;
  roi.height = input_depth_float.rows - (abs(roi_tele_y));
  depthImageAvailable = 0;
//  std::cout << "HIER2 " << roi.height<< "; roi_y :"<<roi_tele_y << std::endl;
}
else
{
  roi.y = roi_tele_y;
  roi.height = tele_crop.height;
  depthImageAvailable = 1;
//  std::cout << "HIER3 "<< tele_crop.height<< std::endl;
}
//std::cout << "roi_x: " << roi.x << ", roi_y: " << roi.y << ", roi_height: " << roi.height << ", roi_width: "<< roi.width << std::endl;
tele_cropped_img = input_depth_float(roi);
//cv::imshow( "image_depth_tele_crop", tele_cropped_img);
//cv::rectangle(input_depth_float,ul_tele,lr_tele,cv::viz::Color(0,0,255),1);
//  cv::imshow( "image_depth", rotated_img);
//  cv::imshow( "image_depth", input_depth_float);

  cv::waitKey(25);

 cv::Mat tele_cropped_img_uc;
 tele_cropped_img.convertTo(tele_cropped_img_uc,CV_16UC1);
 if (depthImageAvailable == 1)
 {
 cv_bridge::CvImagePtr croppedImgPtr(new cv_bridge::CvImage);
 ros::Time time = ros::Time::now();
 croppedImgPtr->encoding = "16UC1";
 croppedImgPtr->header.stamp = time;
 croppedImgPtr->header.frame_id = "camera_rgb_optical_frame";
 croppedImgPtr->image = tele_cropped_img_uc;
 _pub_cropped_depth_image.publish(croppedImgPtr->toImageMsg());
 }
//TODO PCL SHIT
//// input_depth_float.convertTo(tele_cropped_img_uc,CV_16U);
// pcl::PointCloud<pcl::PointXYZ>::Ptr tele_cropped_cloud(new pcl::PointCloud<pcl::PointXYZ>());
// //tele_cropped_cloud = DepthImagetoPCL(tele_cropped_img,intrinsic);
// //pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>());
//// cv::Mat coords(3, tele_cropped_img.cols * tele_cropped_img.rows, CV_64FC1);
//// for (int y=0;y<tele_cropped_img.rows;y++)
//// {
////     for (int x=0;x<tele_cropped_img.cols;x++)
////     {
////         pcl::PointXYZ point;
////         coords.at<double>(0,y*tele_cropped_img.cols+x) = tele_cropped_cloud->points.at(y*tele_cropped_img.cols+x).x;
////         coords.at<double>(1,y*tele_cropped_img.cols+x) = tele_cropped_cloud->points.at(y*tele_cropped_img.cols+x).y;
////         coords.at<double>(2,y*tele_cropped_img.cols+x) = tele_cropped_cloud->points.at(y*tele_cropped_img.cols+x).z;
////     //    tele_cropped_cloud->points.push_back(point);
////     }
//// }
// uint32_t rows = (uint32_t)roi.height;
// uint32_t cols = (uint32_t)roi.width;
//
// tele_cropped_cloud->header.frame_id = "camera_rgb_optical_frame";
// tele_cropped_cloud->height = rows;
// tele_cropped_cloud->width = cols;
// tele_cropped_cloud->is_dense = false;
// tele_cropped_cloud->points.resize(tele_cropped_cloud->width * tele_cropped_cloud->height);
//// std::cout<< "rows:"<< rows << ", cols: " << cols << "points: " << tele_cropped_cloud->size()  <<std::endl;
////double camera_cx_proportional = ((double)cols / (double)orbbec_depth_resX) * depth_camera_cx;
////double camera_cy_proportional = ((double)rows / (double)orbbec_depth_resY) * depth_camera_cy;
////double camera_cx_proportional = (double)center_tele.x;
////double camera_cy_proportional = (double)center_tele.y;
//// double camera_cx_proportional = (double)depth_camera_cx;
//// double camera_cy_proportional = (double)depth_camera_cy;
//double camera_cx_proportional = ((double)tele_crop.width / (double)orbbec_depth_resX) * depth_camera_cx;
//double camera_cy_proportional = ((double)tele_crop.height / (double)orbbec_depth_resY) * depth_camera_cy;
////std::cout << "2: "<<"depth_camera_cx_proportional: " << camera_cx_proportional << ", depth_camera_cy_proportional: " << camera_cy_proportional << std::endl;
//double camera_fx_proportional =  depth_camera_fx;
//double camera_fy_proportional =  depth_camera_fy;
////double camera_cx_proportional = (double)depth_camera_cx;
////double camera_cy_proportional = (double)depth_camera_cy;
////double camera_fx_proportional = depth_camera_fx;
////double camera_fy_proportional = depth_camera_fy;
////std::cout << "cx_prop: " << camera_cx_proportional << ", cy_prop: " << camera_cy_proportional<< ", center_tele_x: "<<center_tele.x <<", center_tele_y: "<< center_tele.y <<", camera_cx: "<< depth_camera_cx <<", camera_cy: "<< depth_camera_cy<< std::endl;
// // for (int m = 0; m < tele_cropped_cloud->height; ++m)
//// {
////   for (int n = 0; n < tele_cropped_cloud->width; ++n)
////   {
////     //ushort d = img.ptr<ushort>(m)[n];
////     //double test = tele_cropped_img.at<ushort>(m,n);
////     pcl::PointXYZ p;
////     p.z = static_cast<float>((tele_cropped_img.at<uint16_t>(m,n)))/10000.0; /// camera_factor;
////     p.x = ((n - camera_cx) * p.z / camera_fx);///1000;
////     p.y = ((m - camera_cy) * p.z / camera_fy);///1000;
////     //tele_cropped_cloud->points.push_back(p);
////
////     //std::cout << "distance: " << test << std::endl;
////     std::cout<<"point("<<m<<", "<<n<< ") = " << p << std::endl;
//////     float X,Y,Z;
//////     Z = static_cast<float>((tele_cropped_img.at<ushort>(m,n))/1000);
//////     X = static_cast<float>((n - camera_cx) * Z / camera_fx);
//////     Y = static_cast<float>((m - camera_cy) * Z / camera_fy);
////     tele_cropped_cloud->at(n,m).z = p.z;
////     tele_cropped_cloud->at(n,m).x = p.x;
////     tele_cropped_cloud->at(n,m).y = p.y;
////   }
//// }
//
////TODO KP ging mal etz nimmer
//double fx = 1.0/depth_camera_fx;
//double fy = 1.0/depth_camera_fy;
//#pragma omp parallel for
// for (unsigned int u = 0; u < rows; u++) {
//   for (unsigned int v = 0; v < cols; v++) {
//     float Xw = 0, Yw = 0, Zw = 0;
//
//     Zw = (float)(tele_cropped_img_uc.at<ushort>(u, v)) / 1000.0;
//
//     Xw = (float) (((double)v - camera_cx_proportional) * (double)Zw * fx);
//     Yw = (float) (((double)u - camera_cy_proportional) * (double)Zw * fy);
//
//     tele_cropped_cloud->at(v, u).x = Xw;
//     tele_cropped_cloud->at(v, u).y = Yw;
//     tele_cropped_cloud->at(v, u).z = Zw;
////     tele_cropped_cloud-
////     std::cout<< "point("<< v <<", "<<u<<") "<<"x: " << Xw << ", y:" << Yw <<", z:" << Zw << std::endl;
//
//
//   }
//}
////TODO ging bis hier
////#pragma omp parallel for
//// for (unsigned int u = 0; u < rows; ++u)
//// {
////   for (unsigned int v = 0; v < cols; ++v)
////   {
////     pcl::PointXYZ cp;
////     float Zw = (float)(tele_cropped_img_uc.at<ushort>(u, v)) / 1000.0;
////     cp.z = Zw;
////     cp.x = (float) (((double)v - camera_cx_proportional) * (double)Zw / depth_camera_fx);
////     cp.y = (float) (((double)u - camera_cy_proportional) * (double)Zw / depth_camera_fy);
////     tele_cropped_cloud->points.push_back(cp);
////
////
////
////   }
////}
//
//// pcl::MonochromeCloud::ConstPtr cloud(new pcl::PointCloud<pcl::PointXYZ>());
//
//
// //TODO
//// pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
//// pcl::VoxelGrid<pcl::PointXYZ> sor;
//// sor.setInputCloud(tele_cropped_cloud);
//// sor.setLeafSize(0.01f, 0.01f, 0.01f);
//// sor.filter(*cloud_filtered);
////
////
////
////
////
// pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
// pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
// pcl::SACSegmentation<pcl::PointXYZ> seg;
// seg.setOptimizeCoefficients (true);
// seg.setModelType (pcl::SACMODEL_PLANE);
// seg.setMethodType (pcl::SAC_RANSAC);
// seg.setDistanceThreshold (0.05);
//
// seg.setInputCloud (tele_cropped_cloud);
// seg.segment (*inliers, *coefficients);
//
//// std::cerr << "Model coefficients: " << coefficients->values[0] << " "
////                                      << coefficients->values[1] << " "
////                                      << coefficients->values[2] << " "
////                                      << coefficients->values[3] << std::endl;
//// std::cerr << "Model inliers: " << inliers->indices.size () << std::endl;
// pcl::PointCloud<pcl::PointXYZ>::Ptr plane_cloud(new pcl::PointCloud<pcl::PointXYZ>());
//  for (size_t i = 0; i < inliers->indices.size (); ++i)
//  {
//    pcl::PointXYZ p;
////    std::cerr << inliers->indices[i] << "    " << tele_cropped_cloud->points[inliers->indices[i]].x << " "
////                                               << tele_cropped_cloud->points[inliers->indices[i]].y << " "
////                                               << tele_cropped_cloud->points[inliers->indices[i]].z << std::endl;
//
//    p.x = tele_cropped_cloud->points[inliers->indices[i]].x;
//    p.y = tele_cropped_cloud->points[inliers->indices[i]].y;
//    p.z = tele_cropped_cloud->points[inliers->indices[i]].z;
//
//    tele_cropped_cloud->points.push_back(p);
//  }
//// pcl::PointCloud<pcl::PointXYZ>::Ptr plane_cloud (new pcl::PointCloud<pcl::PointXYZ>);
//// std::vector<int> inliers;
//// pcl::SampleConsensusModelPlane<pcl::PointXYZ>::Ptr model_p (new pcl::SampleConsensusModelPlane<pcl::PointXYZ> (tele_cropped_cloud));
//// pcl::RandomSampleConsensus<pcl::PointXYZ> ransac (model_p);
//// ransac.setDistanceThreshold (0.001);
//// ransac.computeModel();
//// ransac.getInliers(inliers);
////
//// pcl::copyPointCloud<pcl::PointXYZ>(*tele_cropped_cloud, inliers, *plane_cloud);
//    //
//// pcl::visualization::CloudViewer viewer("Cloud Viewer");
//// viewer.showCloud(tele_cropped_cloud);
//// while (!viewer.wasStopped ())
//// {
//// }
//// pcl::PCLPointCloud2 pcl_cloud2;
//// pcl::toPCLPointCloud2(*tele_cropped_cloud,pcl_cloud2);
//// sensor_msgs::PointCloud2 cropped_cloud2;
//// pcl_conversions::fromPCL(pcl_cloud2,cropped_cloud2);
//// // Create a container for the data.
//  tele_cropped_cloud->header.frame_id = "camera_rgb_optical_frame";
//  pcl_conversions::toPCL(ros::Time::now(), plane_cloud->header.stamp);
//   pcl_conversions::toPCL(ros::Time::now(), tele_cropped_cloud->header.stamp);
//  _pub_cropped_pointcloud.publish(tele_cropped_cloud);
//  _pub_plane_pointcloud.publish(plane_cloud);
//
//  }
//  //std::cout<< "did do something" <<std::endl;
//}
//
////void IrImageCallback(const sensor_msgs::ImageConstPtr& img)
////{
////  cv_bridge::CvImagePtr inImgPtr;
////  inImgPtr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::MONO16);
////  cv::Mat mono16 = inImgPtr->image.clone();
////  cv::Mat mono8 = cv::Mat(mono16.rows,mono16.cols,CV_8U);
//////  std::cout << "hier1" << std::endl;
//////  std::cout << mono16.rows <<", " <<mono16.cols << std::endl;
////// cv::Mat bgr = cv::Mat(mono16.rows,mono16.cols,CV_16UC3);
//////  cv::cvtColor(inImgPtr->image,bgr,cv::COLOR_GRAY2BGR);
//////  cv::cvtColor(bgr,mono8,cv::COLOR_BGR2GRAY);
//////  for(int y = 0; y < mono16.rows; y++)
//////  {
//////    //std::cout << "hier2" << std::endl;
//////    for(int x = 0 ; x < mono16.cols; x++)
//////    {
////// //     std::cout << "point: " << cv::Point(x,y) << ", value: "<<mono16.at<uint16_t>(cv::Point(x,y)) << std::endl;
////////      std::cout << x << std::endl;
//////      mono8.at<uint8_t>(y,x) = (uint8_t)(mono16.at<uint16_t>(cv::Point(x,y)));
//////    }
//////  }
//////  std::cout << "hier4" << std::endl;
////  cv::normalize(mono16,mono8,0.0,255.0,cv::NORM_MINMAX,CV_8U);
////  cv_bridge::CvImagePtr outImgPtr(new cv_bridge::CvImage);
////  ros::Time time = ros::Time::now();
////  outImgPtr->encoding = "mono8";
////  outImgPtr->header.stamp = time;
////  outImgPtr->header.frame_id = "/orbbec/ir255";
////  outImgPtr->image = mono8;
////  _pub_ir255.publish(outImgPtr->toImageMsg());
  }
}
void RGBImageCallback(const sensor_msgs::ImageConstPtr& img)
{
//  std::cout << "RGBCALLBACK!" << std::endl;
  cv_bridge::CvImagePtr inImgPtr;
  inImgPtr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
//  cv::Point2f center;
//  center.x = orbbec_rgb_resX / 2;
//  center.y = orbbec_rgb_resY / 2;
//  cv::Mat rotation = getRotationMatrix2D(center, 180.0, 1.0);
  cv::Mat input_img = inImgPtr->image.clone();
//  cv::Mat rotated_img;
//  cv::Size image_size;
//  image_size.width = orbbec_rgb_resX;
//  image_size.height = orbbec_rgb_resY;
//  cv::warpAffine(inImgPtr->image, rotated_img, rotation, image_size, cv::INTER_LINEAR, cv::BORDER_DEFAULT);
    cv::Point ul;
    ul.x = center.x - 3;
    ul.y = center.y + 3;
    cv::Point lr;
    lr.x = center.x + 3;
    lr.y = center.y -3;
  cv::Scalar color;
  //TODO for orbbec ... get the pointcloud working ... until then:
  //average_distance = 1000.0;// 1m because some value and no working pointcloud
  double viewable_width = tan(orbbec_angleX * M_PI / 180.0) * average_distance;
  double viewable_height = tan(orbbec_angleY * M_PI /180.0) * average_distance;
  //std::cout<< "viwable_width: "<<viewable_width<< std::endl;
  double x_pixel_size_mm = viewable_width/orbbec_rgb_resX;
  double y_pixel_size_mm = viewable_height/orbbec_rgb_resY;
  double pixel_offset = round(camera_distance_mm/y_pixel_size_mm);
  if(isinf(pixel_offset))
  {
    pixel_offset = last_pixel_offset;
  }
  else
  {
    last_pixel_offset = pixel_offset;
  }
  cv::Point centerTele;
  centerTele.x = center.x;
  centerTele.y = center.y -pixel_offset;
  geometry_msgs::Point centerTeleMsg;
  centerTeleMsg.x = (float)centerTele.x;
  centerTeleMsg.y = (float)centerTele.y;
  _pub_projected_tele_center.publish(centerTeleMsg);


//  double viewable_angle_horizontal = calib_factor*(x3H*pow(camera_zoom_value, 3.0) + x2H*pow(camera_zoom_value, 2.0) + x1H*camera_zoom_value + x0H);//(factor_degree_zoom_1*camera_zoom_value) + factor_degree_zoom_0;
//  double viewable_angle_vertical = calib_factor*(x3V*pow(camera_zoom_value, 3.0) + x2V*pow(camera_zoom_value, 2.0) + x1V*camera_zoom_value + x0V);

  double viewable_angle_horizontal = calib_factor*x1Hpot*pow(camera_zoom_times,expHpot);
  double viewable_angle_vertical = calib_factor*x1Vpot*pow(camera_zoom_times, expVpot);
  //  std::cout << "horizontal_angle: " << viewable_angle_horizontal<< ", vertical_angle: " << viewable_angle_vertical << std::endl;

  double viewable_width_tele = tan(viewable_angle_horizontal * M_PI / 180.0) * average_distance;
  double viewable_height_tele = tan(viewable_angle_vertical * M_PI / 180.0) * average_distance;
//  double viewable_height_tele = viewable_width_tele*factor_horizontal_to_vertical;// * average_distance;
  double pixel_width_tele = round(viewable_width_tele/x_pixel_size_mm);
  double pixel_height_tele = round(viewable_height_tele/y_pixel_size_mm);
//  std::cout<< "viewable_width_tele: "<<viewable_width_tele << std::endl;
//  std::cout<< "viewable_height_tele: "<<viewable_height_tele<< std::endl;
//  std::cout<< "pixel_offset: "<<pixel_offset<< std::endl;
//  std::cout<<std::endl;
  cv::Point ul_center_tele;
  ul_center_tele.x = center.x - 2;
  ul_center_tele.y =center.y - pixel_offset - 2;
  cv::Point lr_center_tele;
  lr_center_tele.x = ul_center_tele.x + 4;
  lr_center_tele.y = ul_center_tele.y +4;


  ul_tele.x = center.x - (pixel_width_tele/2);
  ul_tele.y = center.y - pixel_offset + (pixel_height_tele/2);

    lr_tele.x = center.x + (pixel_width_tele/2);
    lr_tele.y = center.y - pixel_offset - (pixel_height_tele/2);
  cv::rectangle(input_img,ul,lr,cv::viz::Color(0,255,0),cv::FILLED);
  cv::rectangle(input_img,ul_tele,lr_tele,cv::viz::Color(0,0,255),1);
  cv::rectangle(input_img,ul_center_tele,lr_center_tele,cv::viz::Color(0,0,255),cv::FILLED);
  if(depthImageAvailable == 0)
  {
    cv::putText(input_img,"depth not available", cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255),1, cv::LINE_8,false);
  }
//  cv::imshow( "image", rotated_img);
 // cv::rectangle(input_img,ul_center_tele,lr_center_tele,cv::viz::Color(0,0,255),CV_FILLED);
//  cv::imshow( "image", input_img);/camera/depth/camera_info
  cv_bridge::CvImagePtr outImgPtr(new cv_bridge::CvImage);
  ros::Time time = ros::Time::now();
  outImgPtr->encoding = "bgr8";
  outImgPtr->header.stamp = time;
  outImgPtr->header.frame_id = "/orbbec/overview";
  outImgPtr->image = input_img;
  _pub_overview.publish(outImgPtr->toImageMsg());
  //cv::waitKey(25);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "ohm_tele_proc");
  ros::NodeHandle n;
  image_transport::ImageTransport it(n);
  image_transport::Subscriber sub_tele = it.subscribe("/tamron/image_raw", 1, TeleImageCallback);
  image_transport::Subscriber sub_depth = it.subscribe("/camera/depth/image_raw", 1, DepthImageCallback);
  image_transport::Subscriber sub_rgb = it.subscribe("/camera/rgb/image_raw", 1, RGBImageCallback);
//  image_transport::Subscriber sub_ir = it.subscribe("/camera/ir/image", 1, IrImageCallback);
  _pub_tele_rect = it.advertise("/tele_cam/image_rect", 1);
  _pub_tele_rect_cropped = it.advertise("/tele_cam/image_rect_cropped", 1);
  _pub_overview = it.advertise("/orbbec/overview", 1);
//  _pub_ir255 = it.advertise("/orbbec/ir255", 1);
//  _pub_cropped_pointcloud = n.advertise<sensor_msgs::PointCloud2>("/cropped_points", 1);
//  _pub_plane_pointcloud = n.advertise<sensor_msgs::PointCloud2>("/plane_points", 1);
  _pub_med_dist_to_plane = n.advertise<std_msgs::Float32>("/med_dist_to_plane", 1);
  _pub_projected_tele_center = n.advertise<geometry_msgs::Point>("/orbbec/projected_tele_center", 1);
  _pub_cropped_depth_image = it.advertise("/orbbec/depth_cropped", 1);
  ros::Subscriber sub_zoom = n.subscribe("camera_zoom", 1, camera_zoom_CB);
  sub_rgbd_rgb_info = n.subscribe("/camera/rgb/camera_info", 1, rgb_info_CB);
//  ros::Subscriber sub_rgbd_rgb_info = n.subscribe("/usb_cam/camera_info", 1, rgb_info_CB);
  sub_rgbd_depth_info = n.subscribe("/camera/depth/camera_info", 1, depth_info_CB);
  ros::spin();
  return 0;
}


