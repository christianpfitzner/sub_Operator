#include "ros/ros.h"
#include "Maxon6WD.h"

using namespace std;

const char node[]= "maxon_6wd_node";

int main(int argc, char **argv)
{
  ros::init(argc, argv, node);

  Maxon6WD robot;
  robot.run();

  return 0;
}
