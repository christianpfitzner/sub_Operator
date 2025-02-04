/*
 * MapperController.h
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_MAPPERCONTROLLER_H_
#define OHM_RQT_SRC_REMOTE_MAPPERCONTROLLER_H_

#include <sensor_msgs/Joy.h>
#include "mappers/IMapper.h"
#include <memory>

namespace ohm_remote
{

class MapperController
{
public:
  static std::shared_ptr<MapperController> getInstance(void);
  virtual ~MapperController();
  void map(const sensor_msgs::Joy& joy);
  void mapImage(void){_mapper->mapImage();}
  bool switchMapper(const IMapper::RemoteType& type, const bool switched = false);
  bool switchMapper(void);
  const IMapper::RemoteType type(void)const{return _mapper->type();}
private:
  MapperController(void);
  ros::NodeHandle _nh;
  std::shared_ptr<IMapper> _mapper;
  std::vector<std::shared_ptr<IMapper> > _mappers;
  static std::shared_ptr<MapperController> _instance;
  IMapper::RemoteType _last;
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_MAPPERCONTROLLER_H_ */
