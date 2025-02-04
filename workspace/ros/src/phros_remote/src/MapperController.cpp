/*
 * MapperController.cpp
 *
 *  Created on: May 8, 2019
 *      Author: phil
 */

#include "MapperController.h"
#include "gui/Hud.h"
#include "mappers/IMapper.h"
#include "mappers/MapperArm.h"
#include "mappers/MapperDrive.h"
#include "mappers/MapperDriveReverse.h"
#include "mappers/MapperHud.h"
#include "mappers/MapperInspect.h"
#include "mappers/PsProfiles.h"
#include "utils/MapperPs4Pad.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <iostream> 
#include <fstream>

namespace phros_remote
{

std::shared_ptr<MapperController> MapperController::_instance(nullptr);

std::shared_ptr<MapperController> MapperController::getInstance(void)
{
  if(!_instance)
    _instance = std::shared_ptr<MapperController>(new MapperController);
  return _instance;
}

MapperController::MapperController(void)
    : _mappers(5, NULL)  //TODO: eeeevil magic number
//,_nh("heini")
{
  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::ARM)]       = std::shared_ptr<MapperArm>(new MapperArm(_nh, &btnActions, &axisActions));
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)]     = std::shared_ptr<MapperDrive>(new MapperDrive(_nh, &btnActions, &axisActions));
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE_REV)] = std::shared_ptr<MapperDriveReverse>(new MapperDriveReverse(_nh, &btnActions, &axisActions));
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::HUD)]       = std::shared_ptr<MapperHud>(new MapperHud());
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::INSPECT)]   = std::shared_ptr<MapperInspect>(new MapperInspect(&btnActions, &axisActions));
  const IMapper::RemoteType typeDrive                                 = IMapper::RemoteType::DRIVE;
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

void MapperController::switchConfig(std::shared_ptr<std_msgs::String>& msg)
{
  try
  {
    std::string filename = "/home/philipp/ohm_ws/src/phros_remote/src/conf/" + msg->data;

    std::cout<<filename<<std::endl;

    std::ifstream t;
    int length;
    t.open(filename);      // open input file
    t.seekg(0, std::ios::end);    // go to the end
    length = t.tellg();           // report location (this is the length)
    t.seekg(0, std::ios::beg);    // go back to the beginning
    char* buffer = new char[length];    // allocate memory for a buffer of appropriate dimension
    t.read(buffer, length);       // read the whole file into the buffer
    t.close();   

    rapidjson::Document doc;
    doc.Parse(buffer);
    delete buffer;    

    if (doc.HasParseError() == 0) { // check for no parse error
      btnActions.mapperSwitch = doc["switchMapper"].GetInt();
      btnActions.shHome = doc["drive"]["SensorHeadHome"].GetInt();
      btnActions.flipperCompact = doc["drive"]["flipperPresetCompact"].GetInt();
      btnActions.flipperW = doc["drive"]["flipperPresetWShape"].GetInt();
      btnActions.flipperUp = doc["drive"]["moveFlipperUp"].GetInt();
      btnActions.flipperDown = doc["drive"]["moveFlipperDown"].GetInt();
      btnActions.flipperFr = doc["drive"]["selectFlipperFrontRight"].GetInt();
      btnActions.flipperFl = doc["drive"]["selectFlipperFrontLeft"].GetInt();
      btnActions.flipperBl = doc["drive"]["selectFlipperBackLeft"].GetInt();
      btnActions.flipperBr = doc["drive"]["selectFlipperBackRight"].GetInt();
      btnActions.driveSwitch1 = doc["drive"]["swithDriveMode1"].GetInt();
      btnActions.driveSwitch2 = doc["drive"]["switchDriveMode2"].GetInt();

      axisActions.moveLeftRight = doc["drive"]["turnleft/right"].GetInt();
      axisActions.moveFwd = doc["drive"]["driveForward"].GetInt();
      axisActions.moveBwd = doc["drive"]["driveBackward"].GetInt();
      axisActions.shPitch = doc["drive"]["sensorHeadPitch"].GetInt();
      axisActions.shYaw = doc["drive"]["sensorHeadYaw"].GetInt();
      axisActions.armFwBw = doc["arm"]["moveArmForward/Backward"].GetInt();
      axisActions.armLeftRight = doc["arm"]["moveArmLeft/Right"].GetInt();
      axisActions.armUpDown = doc["arm"]["moveArmUp/Down"].GetInt();
    } else {
      std::cout<<__PRETTY_FUNCTION__<<doc.GetParseError()<<std::endl;
    }
   
    std::cout<<"no catch"<<std::endl;
  
  }  
  catch (const std::bad_alloc& e)
  {
    std::cout<<"catch: "<<e.what()<<std::endl;
  }
}

void MapperController::map(std::shared_ptr<MapperPsPad>& msg)
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

  _mapper->map(msg);  //_mapper is current mapper
  // lastJoy = joy;
}

bool MapperController::switchMapper(const IMapper::RemoteType& type, const bool switched)
{
  // static IMapper::RemoteType last = _mapper->type();
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
    dynamic_cast<MapperDrive*>(_mapper.get())->setSwitched(switched);
    break;
  }
  case IMapper::RemoteType::DRIVE_REV:
  {
    Hud::getInstance()->showMenu(false);
    std::cout << __PRETTY_FUNCTION__ << " switch to drive reverse " << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE_REV)];
    _mapper->init();
    dynamic_cast<MapperDriveReverse*>(_mapper.get())->setSwitched(switched);
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
    //_mapper->setReset();
    break;
  }
  case IMapper::RemoteType::INSPECT:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to inspect" << std::endl;
    Hud::getInstance()->showMenu(false);
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::INSPECT)];
    break;
  }
  default:
  {
    std::cout << __PRETTY_FUNCTION__ << " Error. Unknown profile selected " << std::endl;
    return false;
  }
  }
  _mapper->setReset(); // TODO: this call is not good in the hud mapper...check if we need it every time
  Hud::getInstance()->setCurrentProfile(type);
  return true;
}

bool MapperController::switchMapper(void)
{
  std::cout << __PRETTY_FUNCTION__ << "quit hud switch back to last " << static_cast<unsigned int>(_last) << std::endl;
  _mapper = _mappers[static_cast<unsigned int>(_last)];
  Hud::getInstance()->showMenu(false);
  Hud::getInstance()->setCurrentProfile(_mapper->type());
  //_mapper->setReset();
  return true;
}

} // namespace phros_remote
