/*
 * MapperHud.h
 *
 *  Created on: May 6, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERHUD_H_
#define OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERHUD_H_

#include "IMapper.h"
#include "mappers/Ps3Profiles.h"

class Hud;

class MapperHud: public IMapper
{
public:
  MapperHud(Hud& hud);
  virtual ~MapperHud();
  virtual void map(const sensor_msgs::Joy& msg);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::HUD;}
  void reset(void);
private:
  Hud& _hud;
  bool _initial;
  sensor_msgs::Joy _lastPs;
};

#endif /* OHM_SCHR_REMOTE_SRC_MAPPERS_MAPPERHUD_H_ */
