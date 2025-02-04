/*
 * MapperDriveReverse.h
 *
 *  Created on: Apr 26, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERDRIVEREVERSE_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERDRIVEREVERSE_H_

#include "MapperBase.h"

namespace ohm_remote
{

class MapperDriveReverse: public MapperBase
{
public:
  MapperDriveReverse(ros::NodeHandle& nh);
  virtual ~MapperDriveReverse();
  virtual void map(const sensor_msgs::Joy& msg);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::DRIVE_REV;}
  virtual void mapImage(void);
  void homing(const unsigned int iters){_homingIters = iters;}
  virtual void init();
  void setSwitched(const bool val){_switched = val;}
private:
  ros::NodeHandle& _nh;
//  ros::Publisher _pubTwist;
//  ros::Publisher _pubFlippers;
//  ros::Publisher _pubSensorHead;
  float _threshSpeedLinear;
  float _threshSpeedAngular;
  float _threshSpeedSensorHead;
  float _speedFlipperManual;
  unsigned int _homingIters;
  double _homingYawRev  ;
  double _homingPitchRev;
   bool _switched = false;
};

}

#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERDRIVEREVERSE_H_ */
