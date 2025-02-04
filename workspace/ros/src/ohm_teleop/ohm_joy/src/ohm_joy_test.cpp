/*
 * ohm_joy_config.cpp
 *
 *  Created on: 11.02.2015
 *      Author: chris
 */




 // ros includes
#include <ros/ros.h>

// qt includes
#include <QtGui>
#include <QApplication>

#include "JoyTest.h"

int main(int argc, char **argv)
{
   ros::init(argc, argv, "joy_test_node");
   ros::NodeHandle nh("~");

   QApplication app(argc, argv);
   JoyTest w;
   w.setNodeHandle(nh);
   w.show();

   app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
   int result = app.exec();

   return result;
}
