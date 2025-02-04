#ifndef PHROS_REMOTE_MAPPERINSPECT_H_
#define PHROS_REMOTE_MAPPERINSPECT_H_


#include "IMapper.h"
#include "MapperBase.h"
#include "utils/SuperImageSubscriber.h"
#include <memory>

namespace phros_remote
{
  class MapperInspect : public MapperBase
  {
    public:
    MapperInspect(MapperController::ButtonActions* btnActions, MapperController::AxisActions* axisActions);
    virtual ~MapperInspect(){}
    private:
  virtual void map(std::shared_ptr<MapperPsPad>& msg);
  virtual const RemoteType type(void)const{return IMapper::RemoteType::INSPECT;}
  virtual void mapImage(void);
  virtual void init(void){}
  private:
    ros::Publisher _pubVistaControl;
    const unsigned int _zoomSpeedMax = 7;  //TODO: make accesible by launch or config
const unsigned int _focusSpeedMax = 2;
const double _deadzoneZoom = 0.01;
const double _deadzoneFocus = 0.01;
float _threshSpeedSensorHead;
std::unique_ptr<SuperImageSubscriber> _subsImgTele;
std::unique_ptr<SuperImageSubscriber> _subsImgTotal;

  MapperController::ButtonActions* _btnActions;
  MapperController::AxisActions* _axisActions;
};
}
#endif

