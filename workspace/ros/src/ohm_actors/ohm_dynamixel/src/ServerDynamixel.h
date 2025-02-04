/*
 * ServerDynamixel.h
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#ifndef ROS_SRC_NEW_DYN_SRC_SERVERDYNAMIXEL_H_
#define ROS_SRC_NEW_DYN_SRC_SERVERDYNAMIXEL_H_

#include <ros/ros.h>

#include "devices/DynamixelDevice.h"

#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>
#include <dynamixel_workbench_msgs/DynamixelStateList.h>

#include <vector>
#include <memory>

namespace ohm_dynamixel
{

class ServerDynamixel
{
public:
  struct DataServo
  {
    enum class ModeServo
    {
      JOINT = 0,
      MULTITURN
    };
    DataServo(const uint8_t id, const std::string topicBase, const uint16_t speed, const uint16_t acceleration, const DynamixelDevice::FlagsPub& flagsPub, ModeServo mode):
      id(id),
      topicBase(topicBase),
      speed(speed),
      acceleration(acceleration),
      flagsPub(flagsPub),
      modeServo(mode){}
    uint8_t id;
    std::string topicBase;
    uint16_t speed;
    uint16_t acceleration;
    DynamixelDevice::FlagsPub flagsPub;
    ModeServo modeServo;
  };
  ServerDynamixel();
  virtual ~ServerDynamixel();
  void timerCallBack(const ros::TimerEvent& event);
private:
  ros::NodeHandle _nh;
  bool readServoData(const std::string& filename, std::vector<DataServo>* const dataServo); //todo: problem with using unique ptr in call
  bool initDynamixel(void);
  void updateServos(void);
  std::vector<std::unique_ptr<DynamixelDevice> > _devices;
  uint8_t _dxlCnt;
  std::unique_ptr<DynamixelWorkbench> _dxlWb;
  std::vector<ros::Subscriber> _subs;
  std::string _device;
  uint32_t _baudRate;
  uint8_t _scanRange;
  std::vector<DataServo> _servoData;
  std::string _configFile;
  ros::Timer _timerMain;
  bool _initialized;
};

} /* namespace ohm_dynamixel */

#endif /* ROS_SRC_NEW_DYN_SRC_SERVERDYNAMIXEL_H_ */
