#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include <stdint.h>
#include <cmath>

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>

#include "SerialCom/SerialCom.h"
#include "SerialCom/CRC_8.h"

class EsconDual
{
private:    //dataelements
    unsigned int _loopRate;
    bool _stamped_used;
    double _factor_ms_rpm;
    double _wheelbase;
    int _rpm_max;

    apps::SerialCom* _serialPort;
    uint8_t* _serialData;
    apps::CRC_8* _checksum;

    ros::Rate* _rate;
    ros::NodeHandle _nh;

    ros::Time _time_last_cmd;
    ros::Duration _dur_max_no_cmd;

    //ros::Publisher _pub;
    ros::Subscriber _subTwist;

public:
    EsconDual();
    virtual ~EsconDual();

    /**
     * @fn void start(const unsigned int frames = 10)
     *
     * @brief
     *
     *
     * @param[in] const unsigned int rate  ->  rate of the working loop in [1/s]
     *
     *
     * @return  void
     */
    void start(const unsigned int rate = 10);

private:    //functions

    /**
     * @fn void run()
     *
     * @brief this function containts the main working loop
     *
     * @param[in,out]  void
     *
     * @return          void
     */
    void run();

    void handleTwistData(double linear, double angular);

    void transmittData(int16_t r_rpm, int16_t l_rpm);

    //void subCallback(const ROS_PACK::MESSAGE& msg);
    void subTwistCallback(const geometry_msgs::Twist& msg);
    void subTwistStampedCallback(const geometry_msgs::TwistStamped& msg);
};

#endif /* TEMPLATE_H_ */
