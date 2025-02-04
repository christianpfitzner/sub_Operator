#include "GetPosition.h"

#include <tf/tf.h>
#include <tf/transform_listener.h>

#include <ohm_perception_msgs/AddVictim.h>

#include <iostream>

ohm_perception_msgs::Victim GetPosition::pos(void)
{
    if (!_nh)
    {
        std::cout << __PRETTY_FUNCTION__ << ": no valid nodehandle." << std::endl;
        return ohm_perception_msgs::Victim();
    }

    tf::StampedTransform T;
    tf::TransformListener listener;
    try
    {
        /* this is not good, i know! */
        listener.waitForTransform("/map", "/simon/base_footprint", ros::Time(0), ros::Duration(5.0));
        listener.lookupTransform("/map",  "/simon/base_footprint", ros::Time(0), T);
    }
    catch (tf::TransformException ex)
    {
        ROS_ERROR("%s", ex.what());
        return ohm_perception_msgs::Victim();
    }

    const tf::Vector3& t  = T.getOrigin();
    const tf::Vector3& r1 = T.getBasis().getRow(0);
    const tf::Vector3& r2 = T.getBasis().getRow(1);
    const tf::Vector3& r3 = T.getBasis().getRow(2);

    ros::Subscriber sub = _nh->subscribe("/simon/scan", 1, &GetPosition::callbackLaserScan, this);
    ros::ServiceClient cli = _nh->serviceClient<ohm_perception_msgs::AddVictim>("/victim/add_victim");
    ros::Rate rate(10);

    while (ros::ok())
    {
        if (_scanReceived)
        {
            std::cout << "laser scan received." << std::endl;

            if (!_scan.ranges.size())
            {
                std::cout << __PRETTY_FUNCTION__ << ": laser scan is empty." << std::endl;
                _scanReceived = false;
                continue;
            }

            float distance = _scan.ranges[_scan.ranges.size() >> 1];

            if (distance == NAN || distance == INFINITY)
            {
                std::cout << __PRETTY_FUNCTION__ << ": laser range is not valid." << std::endl;
                _scanReceived = false;
                continue;
            }

            std::cout << "distance = " << distance << std::endl;
            ohm_perception_msgs::Victim victim;

            victim.pose.position.x = (1.0f * r1.x() + 0.0f * r1.y() + 0.0f * r1.z()) * distance + t.x();
            victim.pose.position.y = (1.0f * r2.x() + 0.0f * r2.y() + 0.0f * r2.z()) * distance + t.y();
            victim.pose.position.z = (1.0f * r3.x() + 0.0f * r3.y() + 0.0f * r3.z()) * distance + t.z();

            std::cout << "victim coords = " << victim.pose.position.x
                      << ", "               << victim.pose.position.y
                      << ", "               << victim.pose.position.z << std::endl;

            ohm_perception_msgs::AddVictim addVictim;
            addVictim.request.victim = victim;

            if (cli.call(addVictim)) {
               return victim;
            }
            else {
               std::cout << "Could not add victim. Service failed. " << std::endl;
            }

//            victim.id = addVictim.response.id;
//            victim.valid = true;
//            victim.checked = true;

            return victim;
        }

        ros::spinOnce();
        rate.sleep();
    }

    return ohm_perception_msgs::Victim();
}

void GetPosition::callbackLaserScan(const sensor_msgs::LaserScan& scan)
{
    _scan = scan;
    _scanReceived = true;
}
