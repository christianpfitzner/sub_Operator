#include "MenuTest.h"
#include <QtWidgets/QApplication>
#include <ros/ros.h>
int main(int argc, char** argv)
{
  ros::init(argc, argv, "menu_test");
  QApplication                    app(argc, argv);
  MenuTest                        menuTest;
  menuTest.show();
  return app.exec();
}