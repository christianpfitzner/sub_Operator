#ifndef ___SENSOR_HEAD_H___
#define ___SENSOR_HEAD_H___

#include <ros/ros.h>
#include <ohm_actors_msgs/Status.h>
#include <ohm_actors_msgs/SensorHeadMode.h>
#include <ohm_actors_msgs/SensorHeadJoy.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/QuaternionStamped.h>
#include <geometry_msgs/Point.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include "ohm_actors_msgs/SetHomingSensHead.h"

#include <std_msgs/Float32.h>

#include <vector>

#include <Eigen/Core>

class SensorHead
{
public:
    SensorHead(ros::NodeHandle& nh);
    virtual ~SensorHead(void){}
    //void callbackTimerMain(const ros::TimerEvent&);
private:

    enum Servo {
        Pitch = 0,
        Yaw,
        CountServo
    };

    enum Mode {
        None = 0,
        ControlledByJoy,
        LookAtPoint,
        BindDirection,
        ControlledByOculus,
        CountMode
    };
    void spinOnce(void);
    bool callbackMode(ohm_actors_msgs::SensorHeadMode::Request& req, ohm_actors_msgs::SensorHeadMode::Response& res);
    void callbackStatusServoPitch(const std_msgs::Float32& msg);
    void callbackStatusServoYaw(const std_msgs::Float32& msg);
    void callbackJoy(const sensor_msgs::Joy& msg);
    void callbackOhmJoy(const ohm_actors_msgs::SensorHeadJoy& msg);
    void callbackOculus(const geometry_msgs::PoseStamped& msg);
    void callbackDirection(const geometry_msgs::QuaternionStamped& msg);
    void callbackLookAtPoint(const geometry_msgs::Point& msg);
    bool callBackSrvSerHoming(ohm_actors_msgs::SetHomingSensHead::Request& req, ohm_actors_msgs::SetHomingSensHead::Response& res);
    void callBackTimerMain(const ros::TimerEvent& ev);


    ros::NodeHandle                    _nh;

    ros::ServiceServer                 _srvMode;
    std::vector<ros::Publisher>        _pubCommand;
    std::vector<ros::ServiceClient>    _srvSpeed;
    std::vector<ros::Publisher>        _pubSpeed;
    std::vector<ros::ServiceClient>    _srvTorque;
    std::vector<ros::Subscriber>       _subServo;
    ros::Subscriber                    _subTarget;
    std::vector<std::string>           _targetTopic;

    ros::Subscriber                    _joy_sensorhead_sub;

    std::string                        _tfSource;
    std::string                        _tfTarget;
    std::string                        _tfSensor;

    std::vector<std::string>           _tfServoSource;
    std::vector<std::string>           _tfServoTarget;

    tf::TransformListener              _listener;
    tf::TransformBroadcaster           _broadcaster;

    uint8_t                            _mode;
    std::vector<ohm_actors_msgs::Status> _servoStatus;

    std::vector<bool> _servoUpdate;
    double _maxSpeed;
    float _directionYaw;
    Eigen::Matrix3f _Rmt;
    geometry_msgs::Point _lookAtPoint;

    double                             _homing_yaw;
    double                             _homing_pitch;
    ros::ServiceServer _srvSetHoming;

    ros::Timer _timerMain;
    std_msgs::Float32 _anglePitch;
    std_msgs::Float32 _angleYaw;
};

#endif
