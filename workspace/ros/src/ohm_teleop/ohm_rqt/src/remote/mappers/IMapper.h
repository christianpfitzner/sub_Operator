/*
 * IMapper.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_IMAPPER_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_IMAPPER_H_

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>

namespace ohm_remote
{

class IMapper
{
public:
  enum class RemoteType
  {
    DRIVE=0,
    ARM,
    DRIVE_REV,
    HUD
  };
  IMapper(){}
  virtual ~IMapper(){}
  virtual void map(const sensor_msgs::Joy& msg) = 0;
  virtual const RemoteType type(void)const = 0;
  virtual void setReset(void) = 0;
  virtual void mapImage(void) = 0;
  virtual void init(void) = 0;
};

}
#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_IMAPPER_H_ */
