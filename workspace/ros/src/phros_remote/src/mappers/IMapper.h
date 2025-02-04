/*
 * IMapper.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_IMAPPER_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_IMAPPER_H_

#include <ros/ros.h>
#include "utils/MapperPsPad.h"

namespace phros_remote
{

class IMapper
{
public:
  enum class RemoteType
  {
    DRIVE=0,
    ARM,
    DRIVE_REV,
    HUD,
    INSPECT
  };
  IMapper(){}
  virtual ~IMapper(){}
  virtual void map(std::shared_ptr<MapperPsPad>& msg) = 0;   //TODO: this argument should be const but the reset method is called in the map method...solution?!
  virtual const RemoteType type(void)const = 0;
  virtual void setReset(void) = 0;
  virtual void mapImage(void) = 0;
  virtual void init(void) = 0;
};

}
#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_IMAPPER_H_ */
