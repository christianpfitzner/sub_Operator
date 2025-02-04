/*
 * MapperHud.h
 *
 *  Created on: May 6, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERHUD_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERHUD_H_

#include "MapperBase.h"
#include "mappers/Ps3Profiles.h"

namespace ohm_remote
{

class MapperHud: public MapperBase
{
public:
  MapperHud(void);
  virtual ~MapperHud();
  virtual void map(const sensor_msgs::Joy& msg);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::HUD;}
  virtual void mapImage(void){}
  virtual void init(void){}
  //void reset(void);
private:
  //bool _initial;
  //sensor_msgs::Joy _lastPs;
};

}
#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERHUD_H_ */
