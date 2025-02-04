#include <ros/ros.h>


#include "EsconDual.h"

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "ohm_escon_dual_controller_node");
    ros::NodeHandle nh("~");

    EsconDual node;
    node.start(50);

}
