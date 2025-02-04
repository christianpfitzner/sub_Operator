#include <ros/ros.h>

#include "SensorHead.h"



int main(int argc, char** argv)
{
    ros::init(argc, argv, "sensor_head");
    ros::NodeHandle nh;
    ros::NodeHandle prvNh("~");
    double rate = 0.0;
    prvNh.param<double>("loop_rate", rate, 20.0);

    SensorHead head(nh);
   // ros::Timer timer = nh.createTimer(ros::Duration(1.0 / rate), &SensorHead::callbackTimerMain, &head);
    ros::spin();
}

