/*
 * MapperArm.h
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERARM_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERARM_H_

#include "MapperBase.h"

namespace phros_remote
{

class MapperArm: public MapperBase
{
public:
  MapperArm(ros::NodeHandle& nh, MapperController::ButtonActions* btnActions, MapperController::AxisActions* axisActions);
  virtual ~MapperArm();
  virtual void map(std::shared_ptr<MapperPsPad>& msg);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::ARM;}
  virtual void mapImage(void);
  virtual void init(void){}
private:
  ros::NodeHandle& _nh;
  ros::Publisher _pubArmCommand;

  MapperController::ButtonActions* _btnActions;
  MapperController::AxisActions* _axisActions;
};

}

#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERARM_H_ */
