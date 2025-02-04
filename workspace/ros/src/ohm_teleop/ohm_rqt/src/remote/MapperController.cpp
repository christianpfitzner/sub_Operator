/*
 * MapperController.cpp
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#include "MapperController.h"
#include "mappers/IMapper.h"
#include "mappers/MapperDrive.h"
#include "mappers/MapperDriveReverse.h"
#include "mappers/MapperHud.h"
#include "mappers/MapperArm.h"
#include "mappers/Ps3Profiles.h"
#include "gui/Hud.h"

namespace ohm_remote
{

std::shared_ptr<MapperController> MapperController::_instance(nullptr);

std::shared_ptr<MapperController> MapperController::getInstance(void)
{
  if(!_instance)
    _instance = std::shared_ptr<MapperController>(new MapperController);
  return _instance;
}

MapperController::MapperController(void):
                                _mappers(4, NULL)
{
  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::ARM)]   = std::shared_ptr<MapperArm>(new MapperArm(_nh));
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)] = std::shared_ptr<MapperDrive>(new MapperDrive(_nh));
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE_REV)] = std::shared_ptr<MapperDriveReverse>(new MapperDriveReverse(_nh));
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::HUD)] = std::shared_ptr<MapperHud>(new MapperHud());
  const IMapper::RemoteType typeDrive = IMapper::RemoteType::DRIVE;
  //  this->switchMapper(IMapper::RemoteType::DRIVE);
  _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)];
  Hud::getInstance()->setCurrentProfile(IMapper::RemoteType::DRIVE);
  _last = IMapper::RemoteType::DRIVE;
  std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
}

MapperController::~MapperController()
{
  // TODO Auto-generated destructor stub
}

void MapperController::map(const sensor_msgs::Joy& joy)
{
//  static ros::Time timerSwitch = ros::Time::now();
//  static std::shared_ptr<IMapper> lastMapper;
//  static sensor_msgs::Joy lastJoy = joy;
//  const double threshSwitchDir = 0.2;
//  static bool switched = false;
//  if(_mapper->type() != IMapper::RemoteType::HUD)
//  {
//    if(!lastJoy.buttons[B_PS] && joy.buttons[B_PS])
//    {
//      std::cout << __PRETTY_FUNCTION__ << " change to hud mapper" << std::endl;
//      lastMapper = _mapper;
//      this->switchMapper(IMapper::RemoteType::HUD);
//    }
//    if(_mapper->type() != IMapper::RemoteType::ARM)
//    {
//      if(joy.buttons[B_L1] && joy.buttons[B_R1])
//      {
//        const double timePressed = (ros::Time::now() - timerSwitch).toSec();
//        if((timePressed > threshSwitchDir) && !switched)
//        {
//          if(_mapper->type() == IMapper::RemoteType::DRIVE)
//            this->switchMapper(IMapper::RemoteType::DRIVE_REV);
//          else if(_mapper->type() == IMapper::RemoteType::DRIVE_REV)
//            this->switchMapper(IMapper::RemoteType::DRIVE);
//          timerSwitch = ros::Time::now();
//          switched = true;
//        }
//      }
//      else
//      {
//        timerSwitch = ros::Time::now();
//        switched = false;
//      }
//    }
//  }
//  else
//  {
//    if(!lastJoy.buttons[B_PS] && joy.buttons[B_PS])
//    {
//      this->switchMapper(lastMapper->type());
//      _mapper = lastMapper;
//    }
//  }
  _mapper->map(joy);
  //lastJoy = joy;
}

bool MapperController::switchMapper(const IMapper::RemoteType& type, const bool switched)
{
  //static IMapper::RemoteType last = _mapper->type();
  //  std::cout << __PRETTY_FUNCTION__ << " " << static_cast<unsigned int>(_mapper->type()) << " != "
  //            << static_cast<unsigned int>(type) << std::endl;
  //  if(!restore)//_mapper->type() == IMapper::RemoteType::HUD)
  //    if(_hud.lastRemoteProfile() == type)
  //      return false;
  if(_mapper->type() == type)
  {
    std::cout << __PRETTY_FUNCTION__ << " same type selected " << std::endl;
    return false;
  }


//  if(_last == type)
//  {
//    std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
//    return false;
//  }
  switch(type)
  {
  case IMapper::RemoteType::DRIVE:
  {
    Hud::getInstance()->showMenu(false);
    std::cout << __PRETTY_FUNCTION__ << " switch to drive" << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)];
    _mapper->init();
    dynamic_cast<MapperDrive* >(_mapper.get())->setSwitched(switched);
    break;
  }
  case IMapper::RemoteType::DRIVE_REV:
  {
    Hud::getInstance()->showMenu(false);
    std::cout << __PRETTY_FUNCTION__ << " switch to drive reverse " << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE_REV)];
    _mapper->init();
    dynamic_cast<MapperDriveReverse* >(_mapper.get())->setSwitched(switched);
    break;
  }
  case IMapper::RemoteType::ARM:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to arm " << std::endl;
    Hud::getInstance()->showMenu(false);
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::ARM)];
    break;
  }
  case IMapper::RemoteType::HUD:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to hud" << std::endl;
    _last = _mapper->type();
    Hud::getInstance()->showMenu(true);
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::HUD)];
    break;
  }
  default:
  {
    std::cout << __PRETTY_FUNCTION__ << " Error. Unknown profile selected " << std::endl;
    return false;
  }
  }
  _mapper->setReset();
  Hud::getInstance()->setCurrentProfile(type);
  return true;
}

bool MapperController::switchMapper(void)
{
  std::cout << __PRETTY_FUNCTION__ << "quit hud switch back to last " << static_cast<unsigned int>(_last) << std::endl;
  _mapper = _mappers[static_cast<unsigned int>(_last)];
  Hud::getInstance()->showMenu(false);
  Hud::getInstance()->setCurrentProfile(_mapper->type());
  _mapper->setReset();
  return true;
}

} /* namespace ohm_remote */
