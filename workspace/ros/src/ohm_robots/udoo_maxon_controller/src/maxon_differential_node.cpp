#include "ros/ros.h"
#include "MaxonDifferentialDrive.h"

using namespace std;

const char node[]= "maxon_differential_node";

int main(int argc, char **argv)
{
  ros::init(argc, argv, node);

  MaxonDifferentialDrive robot;
  robot.run();

  return 0;
}
