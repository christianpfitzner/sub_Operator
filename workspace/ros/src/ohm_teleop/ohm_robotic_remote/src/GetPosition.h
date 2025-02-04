#ifndef __GET_POSITION__
#define __GET_POSITION__

#include <vector>

#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <ohm_perception_msgs/Victim.h>

class GetPosition
{
public:
    GetPosition(ros::NodeHandle* nh) : _nh(nh), _scanReceived(false) { }

    ohm_perception_msgs::Victim pos(void);

private:
    void callbackLaserScan(const sensor_msgs::LaserScan& scan);

    ros::NodeHandle* _nh;
    sensor_msgs::LaserScan _scan;
    bool _scanReceived;
};

#endif
