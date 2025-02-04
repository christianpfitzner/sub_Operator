/*
 * MapperBase.h
 *
 *  Created on: May 17, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_MAPPERS_MAPPERBASE_H_
#define OHM_RQT_SRC_REMOTE_MAPPERS_MAPPERBASE_H_

#include "IMapper.h"
#include <sensor_msgs/Joy.h>

namespace ohm_remote
{

class MapperBase: public IMapper
{
public:
  MapperBase();
  virtual ~MapperBase();
  void setReset(void){_reset = true;}
  virtual void mapImage(void) = 0;
protected:
  sensor_msgs::Joy _last;
  bool _reset;
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_MAPPERS_MAPPERBASE_H_ */
