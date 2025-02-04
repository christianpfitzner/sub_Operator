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
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <math.h>
#include "std_msgs/UInt16.h"

int resX = 1280;
int resY = 720;
image_transport::Publisher _pub;
//double camera_zoom_times = 0;
uint16_t camera_zoom_value = 0;
//Zoom Factors
double z_3 = 0.00000000000276866;
double z_2 = -0.00000002398916892;
double z_1 = 0.00019946657154348;
double z_0 = 0.99960174706693600;

////Camera Matrix Factors
//double fx_3 = 0.00000000424323061;
//double fx_2 = -0.00003900380807579;
//double fx_1 = 0.25586726189903400;
//double fx_0 = 1038.15602428326000000;
//double cx_2 = -0.0000005335535252;
//double cx_1 = 0.00525941480546786;
//double cx_0 = 628.97445490441400000;
//double fy_3 = 0.00000000425634584;
//double fy_2 = -0.00003931866847815;
//double fy_1 = 0.25603648278452800;
//double fy_0 = 1028.69862142871000000;
//double cy_2 = -0.00000066686190312;
//double cy_1 = 0.00207158723260862;
//double cy_0 = 364.64266109012200000;
//
//double fx_2 = 32.44326148101320000;
//double fx_1 = 1076.11305940905000000;
//double fx_0 = -71.88208592954780000;
//double cx_2 = 1.06988868604090000;
//double cx_1 = -16.13922735362400000;
//double cx_0 = 622.51927217401700000;
//double fy_2 = 33.08233337232530000;
//double fy_1 = 1066.86569632509000000;
//double fy_0 = -72.72507181861040000;
//double cy_2 = 0.94186063580652200;
//double cy_1 = -26.51072783015130000;
//double cy_0 = 398.28736173937600000;
////Distortion Matrix Factors
//double k1_4 = 0.00000000000000154;
//double k1_3 = -0.00000000003108945;
//double k1_2 = 0.00000022712367538;
//double k1_1 = -0.00048334838366146;
//double k1_0 = -0.03232272200511260;
//double k2_3 = -0.00000000026745535;
//double k2_2 = 0.00000361052436194;
//double k2_1 = -0.01247743858926470;
//double k2_0 = 6.66928018744367000;
//double t1_3 = -0.00000000000012378;
//double t1_2 = 0.00000000003108945;
//double t1_1 = -0.00000260812583549;
//double t1_0 = -0.00011103067859098;
//double t2_4 = 0.00000000000000002;
//double t2_3 = -0.00000000000065294;
//double t2_2 = 0.00000000534339741;
//double t2_1 = -0.00001268290128771;
//double t2_0 = 0.00174139744878481;

//double k1_2 = 0.31478304647779800;
//double k1_1 = -0.39341701710859500;
//double k1_0 = -0.08869340063904670;
//double k2_2 = -6.23733957210810000;
//double k2_1 = 15.11680534743660000;
//double k2_0 = -7.14330091780917000;
//double t1_3 = 0.00019952328900227;
//double t1_2 = -0.00575579667355077;
//double t1_1 = 0.00938549485426188;
//double t1_0 = -0.00158837503271442;
//double t2_3 = 0.00127394152902420;
//double t2_2 = -0.01606168622771820;
//double t2_1 = 0.04636587261935510;
//double t2_0 = -0.03711065145500920;

//double k1_2 = 0.31478304647779800;
//double k1_1 = -0.39341701710859500;
//double k1_0 = -0.08869340063904670;
//double k2_2 = -6.23733957210810000;
//double k2_1 = 15.11680534743660000;
//double k2_0 = -7.14330091780917000;
//double t1_3 = 0.00019952328900227;
//double t1_2 = -0.00575579667355077;
//double t1_1 = 0.00938549485426188;
//double t1_0 = -0.00158837503271442;
//double t2_3 = 0.0;
//double t2_2 = 0.00029358709533249;
//double t2_1 = -0.01168127634039520;
//double t2_0 = 0.01733779360187840;

//double k1_2 = 0.31478304647779800;
//double k1_1 = -0.39341701710859500;
//double k1_0 = -0.08869340063904670;
//double k2_4 = 0.35125765905239500;
//double k2_3 = -5.28480850299362000;
//double k2_2 = 18.74257576264610000;
//double k2_1 = -26.91887789982190000;
//double k2_0 = 13.90908844033290000;
//double t1_3 = 0.00019952328900227;
//double t1_2 = -0.00575579667355077;
//double t1_1 = 0.00938549485426188;
//double t1_0 = -0.00158837503271442;
//double t2_4 = 0.00005267635724993;
//double t2_3 = 0.00035823968352184;
//double t2_2 = -0.01073433450745720;
//double t2_1 = 0.03444995468166150;
//double t2_0 = -0.02868941838163720;

double fx_2 = 2.12197e+2;
double fx_1 = 4.08579e+2;
double fx_0 = 4.74801e+2;
double cx_3 = -1.06965e+2;
double cx_2 = 5.87311e+2;
double cx_1 = -9.94953e+2;
double cx_0 = 1.15620e+3;
double fy_2 = 2.13135e+2;
double fy_1 = 4.01475e+2;
double fy_0 = 4.72648e+2;
double cy_2 = 1.38073e-1;
double cy_1 = -3.44174e+1;
double cy_0 = 4.04013e+2;

double k1_2 = 4.56057e-1;
double k1_1 = -7.91294e-1;
double k1_0 = 1.27653e-1;
double k2_5 = -6.59777e+0;
double k2_4 = 5.70863e+1;
double k2_3 = -1.92510e+2;
double k2_2 = 3.14290e+2;
double k2_1 = -2.48442e+2;
double k2_0 = 7.61799e+1;
double t1_3 = -1.19500e-2;
double t1_2 = 5.56647e-2;
double t1_1 = -8.22078e-2;
double t1_0 = 3.76973e-2;
double t2_4 = -1.71066e-2;
double t2_3 = 9.59189e-2;
double t2_2 = -1.72445e-1;
double t2_1 = 1.10309e-1;
double t2_0 = -1.78410e-2;

//double fx_2_o9000 = -3.59773e+1;
//double fx_1_o9000 = 1.70396e+3;
//double fx_0_o9000 = -1.26389e+3;
//double cx_2_o9000 = -3.41652e-1;
//double cx_1_o9000 = -7.72773e+0;
//double cx_0_o9000 = 6.55935e+2;
//double fy_2_o9000 = -3.96797e+1;
//double fy_1_o9000 = 1.73151e+3;
//double fy_0_o9000 = -1.33229e+3;
//double cy_2_o9000 = 1.85077e+0;
//double cy_1_o9000 = -2.79160e+0;
//double cy_0_o9000 = 3.75951e+2;
//
//double k1_2_o9000 = 2.93280e-1;
//double k1_1_o9000 = -2.03916e-1;
//double k1_0_o9000 = -3.50501e-1;
//double k2_3_o9000 = -5.58692e+0;
//double k2_2_o9000 = 7.84404e+1;
//double k2_1_o9000 = -3.85763e+2;
//double k2_0_o9000 = 5.59028e+2;
//double t1_2_o9000 = 2.86610e-3;
//double t1_1_o9000 = -5.64378e-2;
//double t1_0_o9000 = 1.15900e-1;
//double t2_3_o9000 = -3.56588e-3;
//double t2_2_o9000 = 5.68172e-2;
//double t2_1_o9000 = -2.86634e-1;
//double t2_0_o9000 = 4.14023e-1;

double fx_2_o9000 = 32.44326148101320000;
double fx_1_o9000 = 1076.11305940905000000;
double fx_0_o9000 = -71.88208592954780000;
double cx_2_o9000 = 1.06988868604090000;
double cx_1_o9000 = -16.13922735362400000;
double cx_0_o9000 = 622.51927217401700000;
double fy_2_o9000 = 33.08233337232530000;
double fy_1_o9000 = 1066.86569632509000000;
double fy_0_o9000 = -72.72507181861040000;
double cy_2_o9000 = 0.94186063580652200;
double cy_1_o9000 = -26.51072783015130000;
double cy_0_o9000 = 398.28736173937600000;

double k1_2_o9000 = 0.31478304647779800;
double k1_1_o9000 = -0.39341701710859500;
double k1_0_o9000 = -0.08869340063904670;
double k2_4_o9000 = 0.35125765905239500;
double k2_3_o9000 = -5.28480850299362000;
double k2_2_o9000 = 18.74257576264610000;
double k2_1_o9000 = -26.91887789982190000;
double k2_0_o9000 = 13.90908844033290000;
double t1_3_o9000 = 0.00019952328900227;
double t1_2_o9000 = -0.00575579667355077;
double t1_1_o9000 = 0.00938549485426188;
double t1_0_o9000 = -0.00158837503271442;
double t2_4_o9000 = 0.00005267635724993;
double t2_3_o9000 = 0.00035823968352184;
double t2_2_o9000 = -0.01073433450745720;
double t2_1_o9000 = 0.03444995468166150;
double t2_0_o9000 = -0.02868941838163720;

void camera_zoom_CB(const std_msgs::UInt16::ConstPtr& camera_zoom)
{
  //double camera_zoom_value = static_cast<double>(camera_zoom->data);
  //camera_zoom_times =(z_3*pow(camera_zoom_value, 3.0))+(z_2*pow(camera_zoom_value, 2.0))+(z_1*camera_zoom_value)+z_0;
  camera_zoom_value = camera_zoom->data;
}

void imageCallback(const sensor_msgs::ImageConstPtr& img)
{

  cv_bridge::CvImagePtr inMsgPtr;
  cv_bridge::CvImagePtr outImgPtr(new cv_bridge::CvImage);
  inMsgPtr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
  cv::Mat cameraMatrix = cv::Mat(3, 3, CV_32FC1);
  //calculation of the Camera Matrix
//  cameraMatrix.at<float>(0,0) = static_cast<float>((fx_3*pow(camera_zoom_value, 3.0))+(fx_2*pow(camera_zoom_value, 2.0))+(fx_1*camera_zoom_value)+fx_0);
//  cameraMatrix.at<float>(0,1) = 0.0;
//  cameraMatrix.at<float>(0,2) = static_cast<float>((cx_2*pow(camera_zoom_value, 2.0))+(cx_1*camera_zoom_value)+cx_0);
//  cameraMatrix.at<float>(1,0) = 0.0;
//  cameraMatrix.at<float>(1,1) = static_cast<float>((fy_3*pow(camera_zoom_value, 3.0))+(fy_2*pow(camera_zoom_value, 2.0))+(fy_1*camera_zoom_value)+fy_0);
//  cameraMatrix.at<float>(1,2) = static_cast<float>((cy_2*pow(camera_zoom_value, 2.0))+(cy_1*camera_zoom_value)+cy_0);
//  cameraMatrix.at<float>(2,0) = 0.0;
//  cameraMatrix.at<float>(2,1) = 0.0;
//  cameraMatrix.at<float>(2,2) = 1.0;

//  cameraMatrix.at<float>(0,0) = static_cast<float>(+(fx_2*pow(camera_zoom_times, 2.0))+(fx_1*camera_zoom_times)+fx_0);
//  cameraMatrix.at<float>(0,1) = 0.0;
//  cameraMatrix.at<float>(0,2) = static_cast<float>((cx_2*pow(camera_zoom_times, 2.0))+(cx_1*camera_zoom_times)+cx_0);
//  cameraMatrix.at<float>(1,0) = 0.0;
//  cameraMatrix.at<float>(1,1) = static_cast<float>(+(fy_2*pow(camera_zoom_times, 2.0))+(fy_1*camera_zoom_times)+fy_0);
//  cameraMatrix.at<float>(1,2) = static_cast<float>((cy_2*pow(camera_zoom_times, 2.0))+(cy_1*camera_zoom_times)+cy_0);
//  cameraMatrix.at<float>(2,0) = 0.0;
//  cameraMatrix.at<float>(2,1) = 0.0;
//  cameraMatrix.at<float>(2,2) = 1.0;

//  cameraMatrix.at<float>(0,0) = 1136.800012;
//  cameraMatrix.at<float>(0,1) = 0.0;
//  cameraMatrix.at<float>(0,2) = 631.942269;
//  cameraMatrix.at<float>(1,0) = 0.0;
//  cameraMatrix.at<float>(1,1) = 1129.103504;
//  cameraMatrix.at<float>(1,2) = 352.739871;
//  cameraMatrix.at<float>(2,0) = 0.0;
//  cameraMatrix.at<float>(2,1) = 0.0;
//  cameraMatrix.at<float>(2,2) = 1.0;

//  cameraMatrix.at<float>(0,0) = 2411.179157;
//  cameraMatrix.at<float>(0,1) = 0.0;
//  cameraMatrix.at<float>(0,2) = 671.722633;
//  cameraMatrix.at<float>(1,0) = 0.0;
//  cameraMatrix.at<float>(1,1) = 2399.157881;
//  cameraMatrix.at<float>(1,2) = 297.155916;
//  cameraMatrix.at<float>(2,0) = 0.0;
//  cameraMatrix.at<float>(2,1) = 0.0;
//  cameraMatrix.at<float>(2,2) = 1.0;
//  if ((camera_zoom_value >= 0) && (camera_zoom_value < 1000))
//  {
//      cameraMatrix.at<float>(0,0) = 1136.800012;
//      cameraMatrix.at<float>(0,1) = 0.0;
//      cameraMatrix.at<float>(0,2) = 631.942269;
//      cameraMatrix.at<float>(1,0) = 0.0;
//      cameraMatrix.at<float>(1,1) = 1129.103504;
//      cameraMatrix.at<float>(1,2) = 352.739871;
//      cameraMatrix.at<float>(2,0) = 0.0;
//      cameraMatrix.at<float>(2,1) = 0.0;
//      cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 1000) && (camera_zoom_value < 2000))
//  {
//    cameraMatrix.at<float>(0,0) = 1222.076761;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 635.626946;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 1210.920158;
//    cameraMatrix.at<float>(1,2) = 380.169977;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 2000) && (camera_zoom_value < 3000))
//  {
//    cameraMatrix.at<float>(0,0) = 1373.095521;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 636.719343;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 1362.086943;
//    cameraMatrix.at<float>(1,2) = 351.029723;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 3000) && (camera_zoom_value < 4000))
//  {
//    cameraMatrix.at<float>(0,0) = 1483.099334;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 616.447299;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 1470.270202;
//    cameraMatrix.at<float>(1,2) = 380.503379;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 4000) && (camera_zoom_value < 5000))
//  {
//    cameraMatrix.at<float>(0,0) = 1651.962135;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 607.212228;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 1636.642702;
//    cameraMatrix.at<float>(1,2) = 360.917015;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 5000) && (camera_zoom_value < 6000))
//  {
//    cameraMatrix.at<float>(0,0) = 1879.43669;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 646.601325;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 1870.219989;
//    cameraMatrix.at<float>(1,2) = 320.941672;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 6000) && (camera_zoom_value < 7000))
//  {
//    cameraMatrix.at<float>(0,0) = 2181.379322;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 702.391616;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 2156.947472;
//    cameraMatrix.at<float>(1,2) = 452.315986;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 7000) && (camera_zoom_value < 8000))
//  {
//    cameraMatrix.at<float>(0,0) = 2411.179157;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 671.722633;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 2399.157881;
//    cameraMatrix.at<float>(1,2) = 297.155916;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 8000) && (camera_zoom_value < 9000))
//  {
//    cameraMatrix.at<float>(0,0) = 2739.810788;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 672.045125;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 2723.060755;
//    cameraMatrix.at<float>(1,2) = 353.828865;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 9000) && (camera_zoom_value < 10000))
//  {
//    cameraMatrix.at<float>(0,0) = 3405.456291;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 608.178521;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 3389.771996;
//    cameraMatrix.at<float>(1,2) = 298.934966;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 10000) && (camera_zoom_value < 11000))
//  {
//    cameraMatrix.at<float>(0,0) = 3904.392704;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 552.860331;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 3874.660526;
//    cameraMatrix.at<float>(1,2) = 285.294646;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 11000) && (camera_zoom_value < 12000))
//  {
//    cameraMatrix.at<float>(0,0) = 4879.664229;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 650.456364;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 4838.8686;
//    cameraMatrix.at<float>(1,2) = 308.552878;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 12000) && (camera_zoom_value < 13000))
//  {
//    cameraMatrix.at<float>(0,0) = 5686.807449;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 585.936759;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 5636.450392;
//    cameraMatrix.at<float>(1,2) = 336.656243;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 13000) && (camera_zoom_value < 14000))
//  {
//    cameraMatrix.at<float>(0,0) = 7240.907777;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 636.153771;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 7165.534257;
//    cameraMatrix.at<float>(1,2) = 275.067339;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 14000) && (camera_zoom_value < 15000))
//  {
//    cameraMatrix.at<float>(0,0) = 8368.179549;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 596.321284;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 8382.947125;
//    cameraMatrix.at<float>(1,2) = 247.075822;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 15000) && (camera_zoom_value < 16000))
//  {
//    cameraMatrix.at<float>(0,0) = 10633.862089;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 611.47631;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 10575.169111;
//    cameraMatrix.at<float>(1,2) = 250.973693;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }
//  else if ((camera_zoom_value >= 16000) && (camera_zoom_value < 17000))
//  {
//    cameraMatrix.at<float>(0,0) = 11243.204137;
//    cameraMatrix.at<float>(0,1) = 0.0;
//    cameraMatrix.at<float>(0,2) = 523.361095;
//    cameraMatrix.at<float>(1,0) = 0.0;
//    cameraMatrix.at<float>(1,1) = 11080.558223;
//    cameraMatrix.at<float>(1,2) = 294.224786;
//    cameraMatrix.at<float>(2,0) = 0.0;
//    cameraMatrix.at<float>(2,1) = 0.0;
//    cameraMatrix.at<float>(2,2) = 1.0;
//  }

  cv::Mat distortion(1, 5, CV_32FC1);
//  calculation of the Distortion Matrix
//  distortion.at<float>(0,0) = static_cast<float>((k1_4*pow(camera_zoom_times, 4.0))+(k1_3*pow(camera_zoom_times, 3.0))+(k1_2*pow(camera_zoom_times, 2.0))+(k1_1*camera_zoom_times)+k1_0);
//  distortion.at<float>(0,1) = static_cast<float>((k2_3*pow(camera_zoom_times, 3.0))+(k2_2*pow(camera_zoom_times, 2.0))+(k2_1*camera_zoom_times)+k2_0);
//  distortion.at<float>(0,2) = static_cast<float>((t1_3*pow(camera_zoom_times, 3.0))+(t1_2*pow(camera_zoom_times, 2.0))+(t1_1*camera_zoom_times)+t1_0);
//  distortion.at<float>(0,3) = static_cast<float>((t2_4*pow(camera_zoom_times, 4.0))+(t2_3*pow(camera_zoom_times, 3.0))+(t2_2*pow(camera_zoom_times, 2.0))+(t2_1*camera_zoom_times)+t2_0);
//  distortion.at<float>(0,4) = 0.0;
//  distortion.at<float>(0,0) = static_cast<float>((k1_2*pow(camera_zoom_times, 2.0))+(k1_1*camera_zoom_times)+k1_0);
//  distortion.at<float>(0,1) = static_cast<float>((k2_4*pow(camera_zoom_times, 4.0))+(k2_3*pow(camera_zoom_times, 3.0))+(k2_2*pow(camera_zoom_times, 2.0))+(k2_1*camera_zoom_times)+k2_0);
//  distortion.at<float>(0,2) = static_cast<float>((t1_3*pow(camera_zoom_times, 3.0))+(t1_2*pow(camera_zoom_times, 2.0))+(t1_1*camera_zoom_times)+t1_0);
//  distortion.at<float>(0,3) = static_cast<float>((t2_4*pow(camera_zoom_times, 4.0))+(t2_3*pow(camera_zoom_times, 3.0))+(t2_2*pow(camera_zoom_times, 2.0))+(t2_1*camera_zoom_times)+t2_0);
//  distortion.at<float>(0,4) = 0.0;

//  distortion.at<float>(0,0) = -0.226616;
//  distortion.at<float>(0,1) = 0.000779;
//  distortion.at<float>(0,2) = -0.001176;
//  distortion.at<float>(0,3) = -0.001949;
//  distortion.at<float>(0,4) = 0.0;

//  distortion.at<float>(0,0) = 0.577564;
//  distortion.at<float>(0,1) = -1.756643;
//  distortion.at<float>(0,2) = -0.008618;
//  distortion.at<float>(0,3) = 0.011108;
//  distortion.at<float>(0,4) = 0.0;

//  distortion.at<float>(0,1) = 0.0;
//  distortion.at<float>(0,2) = 0.0;
//  distortion.at<float>(0,3) = 0.0;
//  distortion.at<float>(0,4) = 0.0;
//  distortion.at<float>(0,5) = 0.0;
//  if ((camera_zoom_value >= 0) && (camera_zoom_value < 1000))
//    {
//        distortion.at<float>(0,0) = -0.226616;
//        distortion.at<float>(0,1) = 0.000779;
//        distortion.at<float>(0,2) = -0.001176;
//        distortion.at<float>(0,3) = -0.001949;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 1000) && (camera_zoom_value < 2000))
//    {
//        distortion.at<float>(0,0) = -0.156306;
//        distortion.at<float>(0,1) = -0.049995;
//        distortion.at<float>(0,2) = -0.003206;
//        distortion.at<float>(0,3) = -0.002015;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 2000) && (camera_zoom_value < 3000))
//    {
//        distortion.at<float>(0,0) = -0.122544;
//        distortion.at<float>(0,1) = -0.067375;
//        distortion.at<float>(0,2) = 0.000738;
//        distortion.at<float>(0,3) = -0.002612;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 3000) && (camera_zoom_value < 4000))
//    {
//        distortion.at<float>(0,0) = -0.062257;
//        distortion.at<float>(0,1) = -0.128341;
//        distortion.at<float>(0,2) = 0.000333;
//        distortion.at<float>(0,3) = -0.004381;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 4000) && (camera_zoom_value < 5000))
//    {
//        distortion.at<float>(0,0) = 0.025539;
//        distortion.at<float>(0,1) = -0.264538;
//        distortion.at<float>(0,2) = 0.001096;
//        distortion.at<float>(0,3) = -0.006142;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 5000) && (camera_zoom_value < 6000))
//    {
//        distortion.at<float>(0,0) = 0.161964;
//        distortion.at<float>(0,1) = -0.685675;
//        distortion.at<float>(0,2) = -0.000133;
//        distortion.at<float>(0,3) = 0.003226;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 6000) && (camera_zoom_value < 7000))
//    {
//        distortion.at<float>(0,0) = 0.325953;
//        distortion.at<float>(0,1) = -0.810679;
//        distortion.at<float>(0,2) = 0.011546;
//        distortion.at<float>(0,3) = 0.003341;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 7000) && (camera_zoom_value < 8000))
//    {
//        distortion.at<float>(0,0) = 0.577564;
//        distortion.at<float>(0,1) = -1.756643;
//        distortion.at<float>(0,2) = -0.008618;
//        distortion.at<float>(0,3) = 0.011108;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 8000) && (camera_zoom_value < 9000))
//    {
//        distortion.at<float>(0,0) = 0.89535;
//        distortion.at<float>(0,1) = -2.798897;
//        distortion.at<float>(0,2) = 0.00169;
//        distortion.at<float>(0,3) = 0.010286;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 9000) && (camera_zoom_value < 10000))
//    {
//        distortion.at<float>(0,0) = 1.645671;
//        distortion.at<float>(0,1) = -10.591938;
//        distortion.at<float>(0,2) = -0.024145;
//        distortion.at<float>(0,3) = -0.014602;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 10000) && (camera_zoom_value < 11000))
//    {
//        distortion.at<float>(0,0) = 2.299575;
//        distortion.at<float>(0,1) = -19.305696;
//        distortion.at<float>(0,2) = -0.047921;
//        distortion.at<float>(0,3) = -0.052875;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 11000) && (camera_zoom_value < 12000))
//    {
//        distortion.at<float>(0,0) = 3.687343;
//        distortion.at<float>(0,1) = -54.329039;
//        distortion.at<float>(0,2) = -0.051649;
//        distortion.at<float>(0,3) = 0.020241;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 12000) && (camera_zoom_value < 13000))
//    {
//        distortion.at<float>(0,0) = 4.744563;
//        distortion.at<float>(0,1) = -65.588482;
//        distortion.at<float>(0,2) = -0.04367;
//        distortion.at<float>(0,3) = -0.058143;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 13000) && (camera_zoom_value < 14000))
//    {
//        distortion.at<float>(0,0) = 8.434327;
//        distortion.at<float>(0,1) = -221.967012;
//        distortion.at<float>(0,2) = -0.115698;
//        distortion.at<float>(0,3) = -0.005702;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 14000) && (camera_zoom_value < 15000))
//    {
//        distortion.at<float>(0,0) = 10.755104;
//        distortion.at<float>(0,1) = -208.816495;
//        distortion.at<float>(0,2) = -0.141349;
//        distortion.at<float>(0,3) = -0.064646;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 15000) && (camera_zoom_value < 16000))
//    {
//        distortion.at<float>(0,0) = 17.019863;
//        distortion.at<float>(0,1) = -266.739807;
//        distortion.at<float>(0,2) = -0.189088;
//        distortion.at<float>(0,3) = -0.04346;
//        distortion.at<float>(0,4) = 0.0;
//    }
//  else if ((camera_zoom_value >= 16000) && (camera_zoom_value < 17000))
//    {
//        distortion.at<float>(0,0) = 23.431224 ;
//        distortion.at<float>(0,1) = -802.641467;
//        distortion.at<float>(0,2) = -0.138307;
//        distortion.at<float>(0,3) = -0.224846;
//        distortion.at<float>(0,4) = 0.0;
//    }
  std::cout<< camera_zoom_value << std::endl;
  double camera_zoom_times =(z_3*pow(camera_zoom_value, 3.0))+(z_2*pow(camera_zoom_value, 2.0))+(z_1*camera_zoom_value)+z_0;
  std::cout<< camera_zoom_times << std::endl;
  if (camera_zoom_value < 9000)
  {

      cameraMatrix.at<float>(0,0) = static_cast<float>((fx_2*pow(camera_zoom_times, 2.0))+(fx_1*camera_zoom_times)+fx_0);
      cameraMatrix.at<float>(0,1) = 0.0;
      cameraMatrix.at<float>(0,2) = static_cast<float>((cx_3*pow(camera_zoom_times, 3.0))+(cx_2*pow(camera_zoom_times, 2.0))+(cx_1*camera_zoom_times)+cx_0);
      cameraMatrix.at<float>(1,0) = 0.0;
      cameraMatrix.at<float>(1,1) = static_cast<float>((fy_2*pow(camera_zoom_times, 2.0))+(fy_1*camera_zoom_times)+fy_0);
      cameraMatrix.at<float>(1,2) = static_cast<float>((cy_2*pow(camera_zoom_times, 2.0))+(cy_1*camera_zoom_times)+cy_0);
      cameraMatrix.at<float>(2,0) = 0.0;
      cameraMatrix.at<float>(2,1) = 0.0;
      cameraMatrix.at<float>(2,2) = 1.0;

      distortion.at<float>(0,0) = static_cast<float>((k1_2*pow(camera_zoom_times, 2.0))+(k1_1*camera_zoom_times)+k1_0);
      distortion.at<float>(0,1) = static_cast<float>((k2_5*pow(camera_zoom_times, 5.0))+(k2_4*pow(camera_zoom_times, 4.0))+(k2_3*pow(camera_zoom_times, 3.0))+(k2_2*pow(camera_zoom_times, 2.0))+(k2_1*camera_zoom_times)+k2_0);
      distortion.at<float>(0,2) = static_cast<float>((t1_3*pow(camera_zoom_times, 3.0))+(t1_2*pow(camera_zoom_times, 2.0))+(t1_1*camera_zoom_times)+t1_0);
      distortion.at<float>(0,3) = static_cast<float>((t2_4*pow(camera_zoom_times, 4.0))+(t2_3*pow(camera_zoom_times, 3.0))+(t2_2*pow(camera_zoom_times, 2.0))+(t2_1*camera_zoom_times)+t2_0);
      distortion.at<float>(0,4) = 0.0;
  }
  else if ((camera_zoom_value >= 9000) && (camera_zoom_value < 17000))
  {
    cameraMatrix.at<float>(0,0) = static_cast<float>((fx_2_o9000*pow(camera_zoom_times, 2.0))+(fx_1_o9000*camera_zoom_times)+fx_0_o9000);
    cameraMatrix.at<float>(0,1) = 0.0;
    cameraMatrix.at<float>(0,2) = static_cast<float>((cx_2_o9000*pow(camera_zoom_times, 2.0))+(cx_1_o9000*camera_zoom_times)+cx_0_o9000);
    cameraMatrix.at<float>(1,0) = 0.0;
    cameraMatrix.at<float>(1,1) = static_cast<float>((fy_2_o9000*pow(camera_zoom_times, 2.0))+(fy_1_o9000*camera_zoom_times)+fy_0_o9000);
    cameraMatrix.at<float>(1,2) = static_cast<float>((cy_2_o9000*pow(camera_zoom_times, 2.0))+(cy_1_o9000*camera_zoom_times)+cy_0_o9000);
    cameraMatrix.at<float>(2,0) = 0.0;
    cameraMatrix.at<float>(2,1) = 0.0;
    cameraMatrix.at<float>(2,2) = 1.0;

//    distortion.at<float>(0,0) = static_cast<float>((k1_2_o9000*pow(camera_zoom_times, 2.0))+(k1_1_o9000*camera_zoom_times)+k1_0_o9000);
//    distortion.at<float>(0,1) = static_cast<float>((k2_3_o9000*pow(camera_zoom_times, 3.0))+(k2_2_o9000*pow(camera_zoom_times, 2.0))+(k2_1_o9000*camera_zoom_times)+k2_0_o9000);
//    distortion.at<float>(0,2) = static_cast<float>((t1_2_o9000*pow(camera_zoom_times, 2.0))+(t1_1_o9000*camera_zoom_times)+t1_0_o9000);
//    distortion.at<float>(0,3) = static_cast<float>((t2_3_o9000*pow(camera_zoom_times, 3.0))+(t2_2_o9000*pow(camera_zoom_times, 2.0))+(t2_1_o9000*camera_zoom_times)+t2_0_o9000);
//    distortion.at<float>(0,4) = 0.0;
      distortion.at<float>(0,0) = static_cast<float>((k1_2_o9000*pow(camera_zoom_times, 2.0))+(k1_1_o9000*camera_zoom_times)+k1_0_o9000);
      distortion.at<float>(0,1) = static_cast<float>((k2_4_o9000*pow(camera_zoom_times, 4.0))+(k2_3_o9000*pow(camera_zoom_times, 3.0))+(k2_2_o9000*pow(camera_zoom_times, 2.0))+(k2_1_o9000*camera_zoom_times)+k2_0_o9000);
      distortion.at<float>(0,2) = static_cast<float>((t1_3_o9000*pow(camera_zoom_times, 3.0))+(t1_2_o9000*pow(camera_zoom_times, 2.0))+(t1_1_o9000*camera_zoom_times)+t1_0_o9000);
      distortion.at<float>(0,3) = static_cast<float>((t2_4_o9000*pow(camera_zoom_times, 4.0))+(t2_3_o9000*pow(camera_zoom_times, 3.0))+(t2_2_o9000*pow(camera_zoom_times, 2.0))+(t2_1_o9000*camera_zoom_times)+t2_0_o9000);
      distortion.at<float>(0,4) = 0.0;
  }
  else
  {
        cameraMatrix.at<float>(0,0) = 1.0;
        cameraMatrix.at<float>(0,1) = 0.0;
        cameraMatrix.at<float>(0,2) = 1.0;
        cameraMatrix.at<float>(1,0) = 0.0;
        cameraMatrix.at<float>(1,1) = 640.0;
        cameraMatrix.at<float>(1,2) = 360.0;
        cameraMatrix.at<float>(2,0) = 0.0;
        cameraMatrix.at<float>(2,1) = 0.0;
        cameraMatrix.at<float>(2,2) = 1.0;

                distortion.at<float>(0,0) = 0.0;
                distortion.at<float>(0,1) = 0.0;
                distortion.at<float>(0,2) = 0.0;
                distortion.at<float>(0,3) = 0.0;
                distortion.at<float>(0,4) = 0.0;
  }
  std::cout << cameraMatrix << std::endl;
  std::cout << distortion << std::endl;
  cv::Mat output_image;
  cv::undistort(inMsgPtr->image,output_image,cameraMatrix,distortion,cameraMatrix);
  ros::Time time = ros::Time::now();
  outImgPtr->encoding = "bgr8";
  outImgPtr->header.stamp = time;
  outImgPtr->header.frame_id = "/tele_rect";
  outImgPtr->image = output_image;
//  std::cout << outImgPtr->image << std::endl;
  _pub.publish(outImgPtr->toImageMsg());
//  cv::imshow( "image", output_image );
 // cv::waitKey(25);
//std::cout<< "did do something" <<std::endl;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "ohm_tele_proc");
  ros::NodeHandle n;
  image_transport::ImageTransport it(n);
  image_transport::Subscriber sub = it.subscribe("/usb_cam/image_raw", 1, imageCallback);
  _pub = it.advertise("/tele_cam/image_rect", 1);
  ros::Subscriber sub_zoom = n.subscribe("camera_zoom", 1, camera_zoom_CB);












  ros::spin();



  return 0;
}


