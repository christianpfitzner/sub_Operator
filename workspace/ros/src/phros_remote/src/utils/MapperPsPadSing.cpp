#include "MapperPsPadSing.h"
#include "MapperPs3Pad.h"
#include "MapperPs4Pad.h"
#include <ros/ros.h>
#include <string>

namespace phros_remote
{
  std::unique_ptr<MapperPsPadSing> MapperPsPadSing::_instance;

  MapperPsPadSing::MapperPsPadSing()
  {
    std::string configGamePad;
    ros::NodeHandle prvNh("~");
    prvNh.param<std::string>("config_game_pad", configGamePad, "ps_4");

    if(configGamePad == typePs3)
    {
      ROS_INFO_STREAM(__PRETTY_FUNCTION__ << " Ps3 Gamepad selected");
      _psPad = std::make_shared<MapperPs3Pad>();
    }
    else if(configGamePad == typePs4)
    {
      ROS_INFO_STREAM(__PRETTY_FUNCTION__ << " Ps4 Gamepad selected");
      _psPad = std::make_shared<MapperPs4Pad>();
    }
    else
    {
      ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " unknown type " << configGamePad << " selected! Possible types are\n\t" << typePs3 << "\n\t" 
      << typePs4 << "\n\t\tSetting default: " << typePs3);
      _psPad = std::make_shared<MapperPs3Pad>();
    }
  }

  MapperPsPadSing& MapperPsPadSing::instance()
  {
    if(!_instance)
      _instance = std::unique_ptr<MapperPsPadSing>(new MapperPsPadSing());
    return *_instance;  
  }

  
}