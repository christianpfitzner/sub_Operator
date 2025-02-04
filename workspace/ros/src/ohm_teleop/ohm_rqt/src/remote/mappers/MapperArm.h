/*
 * MapperArm.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERARM_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERARM_H_

#include "MapperBase.h"

namespace ohm_remote
{

class MapperArm: public MapperBase
{
public:
  MapperArm(ros::NodeHandle& nh);
  virtual ~MapperArm();
  virtual void map(const sensor_msgs::Joy& joy);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::ARM;}
  virtual void mapImage(void);
  virtual void init(void){}
private:
  ros::NodeHandle& _nh;
  ros::Publisher _pubArmCommand;
};

}

#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERARM_H_ */
