/*
 * test.aekcpp
 *
 *  Created on: May 6, 2017
 *      Author: phil
 */


#include <ros/ros.h>
#include <dynamixel_controllers/SetSpeed.h>
#include <dynamixel_msgs/JointState.h>
#include <ohm_actors_msgs/SensorHeadJoy.h>
#include <std_msgs/Float64.h>

void callBbackAngle(const ohm_actors_msgs::SensorHeadJoy& angle);
void callBackState(const dynamixel_msgs::JointState& state);

static double _angleCur = 0.0;
static double _angleDes = 0.0;

#define P 2000.0

static ros::ServiceClient _setSpeed;
static ros::Publisher _pubAngle;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "test");
  ros::NodeHandle nh;

  _setSpeed = nh.serviceClient<dynamixel_controllers::SetSpeed>("/sensor_head_pan/set_speed", 1);
  ros::Subscriber subsAngle = nh.subscribe("sensorhead_joy", 1, callBbackAngle);
  ros::Subscriber subsState = nh.subscribe("sensor_head_pan/state", 1, callBackState);
  _pubAngle = nh.advertise<std_msgs::Float64>("sensor_head_pan/command", 1);
  ros::spin();
}


void callBbackAngle(const ohm_actors_msgs::SensorHeadJoy& angle)
{
  std::cout << __PRETTY_FUNCTION__ << " des = " << angle.yaw << std::endl;
  _angleDes = angle.yaw;
}

void callBackState(const dynamixel_msgs::JointState& state)
{
  std::cout << __PRETTY_FUNCTION__ << " des - cur " << (_angleDes - _angleCur) << std::endl;
  _angleCur = state.current_pos;
  dynamixel_controllers::SetSpeed setSpeedCl;
  setSpeedCl.request.speed = (_angleDes - _angleCur) * P;
  std::cout << __PRETTY_FUNCTION__ << " set speed to " << setSpeedCl.request.speed << std::endl;
  if(!_setSpeed.call(setSpeedCl))
  {
    ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << "couldnt call set speed service fuck off");
    return;
  }
  std_msgs::Float64 fakeAngle;
  fakeAngle.data = 4095.0;
  _pubAngle.publish(fakeAngle);
}
