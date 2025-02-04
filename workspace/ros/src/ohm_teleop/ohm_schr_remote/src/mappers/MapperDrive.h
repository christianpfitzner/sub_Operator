/*
 * MapperDrive.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERDRIVE_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERDRIVE_H_

#include "IMapper.h"

#include <ros/ros.h>

class MapperDrive: public IMapper
{
public:
  MapperDrive(ros::NodeHandle& nh);
  virtual ~MapperDrive();
  void map(const sensor_msgs::Joy& joy);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::DRIVE;}
  void homing(const unsigned int iters){_homingIters = iters;}
private:
  ros::NodeHandle& _nh;
  ros::Publisher _pubTwist;
  ros::Publisher _pubFlippers;
  ros::Publisher _pubSensorHead;
  float _threshSpeedLinear;
  float _threshSpeedAngular;
  float _threshSpeedSensorHead;
  float _speedFlipperManual;
  unsigned int _homingIters;
  ros::ServiceClient _clientMap;
};

#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERDRIVE_H_ */
