#include "MainWindow.h"

#include <QApplication>
#include <ros/ros.h>

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

   char argv0[] = "RosThread";
   char argv1[] = "_image_transport:=compressed";
   char* rosArgv[2] = { argv0 , argv1 };
   int rosArgc = 2;

   ros::init(rosArgc, rosArgv, "Gui");

   MainWindow gui;
   gui.show();

   return app.exec();
}
