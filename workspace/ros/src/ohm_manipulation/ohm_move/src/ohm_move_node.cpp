#include <iostream>
#include <ros/ros.h>
#include "move.h"

std::shared_ptr<Move> makeAchange;


int main(int argc, char **argv) {

  ros::init(argc, argv, "ohm_move_node");

  makeAchange = std::make_shared<Move>();

  constexpr double hz = 100;
  ros::Rate rate(hz);

  ros::spin();

  return 0;
}
