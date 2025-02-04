/*
 * ServerDynamixel.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: phil
 */

#include "ServerDynamixel.h"

#include "devices/DynamixelDevice.h"
#include "devices/DynamixelAX12A.h"
#include "devices/DynamixelMX28.h"
#include "devices/DynamixelXH430V350.h"
#include "devices/DynamixelMX106.h"
#include "devices/DynamixelXH540V270.h"
#include "devices/DeviceData.h"

#include <tinyxml2.h>

#include <ros/ros.h>

namespace ohm_dynamixel
{

ServerDynamixel::ServerDynamixel():
                _dxlWb(new DynamixelWorkbench),
                _scanRange(0),
                _baudRate(0),
                _dxlCnt(0),
                _initialized(false)
{

  _timerMain = _nh.createTimer(ros::Duration(1.0 / 200.0), &ServerDynamixel::timerCallBack, this);
  ros::NodeHandle prvNh("~");
  prvNh.param<std::string>("config_file", _configFile, "/home/phil/workspace/phillib/ros/src/new_dyn/config/bus.xml");
}

ServerDynamixel::~ServerDynamixel()
{

}

bool ServerDynamixel::initDynamixel(void)
{
  bool success = false;
//  std::unique_ptr<std::vector<DataServo> > servoDatas;//(new std::vector<DataServo>);  todo: problem with using unique ptr instead of const ptr
//  if(!this->readServoData(_configFile, servoDatas))
  std::vector<DataServo> servoData;
  if(!this->readServoData(_configFile, &servoData))
  {
    ROS_ERROR("%s - error: Reading config file at %s", __PRETTY_FUNCTION__, _configFile.c_str());
    ros::shutdown();
    std::exit(1);
  }
  _dxlWb->begin(_device.c_str(), _baudRate);
  std::vector<uint8_t> dxlIds(16, 0);

  if(_dxlWb->scan(dxlIds.data(), &_dxlCnt, _scanRange) != true)
  {
    ROS_ERROR("Could not find dynamixels! Please check scan range and baud rate");
    ros::shutdown();
    std::exit(1);
  }
  ROS_INFO("Found %i devices", static_cast<unsigned int>(_dxlCnt));
  unsigned int i = 0;
  for(auto& iterId : dxlIds)
  {
    uint8_t id = iterId;///_dxlId[i];
    const DataServo* data = NULL;   //todo: unique_ptr
    for(auto& iterData : servoData)
    {
      if(id == iterData.id)
      {
        data = &iterData;
        break;
      }
    }
    if(data)
    {
      std::string model;
        model = std::string(_dxlWb->getModelName(iterId));
      std::unique_ptr<DynamixelDevice> device(nullptr);
      if(model == AX12ATYPE)
        device = std::unique_ptr<DynamixelAX12A>(new DynamixelAX12A(id, data->speed, data->topicBase, _nh, data->flagsPub));
      else if(model == MX28TYPE)
        device = std::unique_ptr<DynamixelMX28>(new DynamixelMX28(id, data->speed, data->topicBase, _nh, data->flagsPub));
      else if(model == XH430V350TYPE)
        device = std::unique_ptr<DynamixelXH430V350>(new DynamixelXH430V350(id, data->speed, data->topicBase, _nh, data->flagsPub));
      else if(model == MX106TYPE)
        device = std::unique_ptr<DynamixelMX106>(new DynamixelMX106(id, data->speed, data->topicBase, _nh, data->flagsPub));
      else if(model == XH540V270TYPE)
        device = std::unique_ptr<DynamixelXH540V270>(new DynamixelXH540V270(id, data->speed, data->topicBase, _nh, data->flagsPub));
      else
        ROS_ERROR("%s - error: Unknown device %s", __PRETTY_FUNCTION__, model.c_str());
      //std::cout << "MODEL   : " << _dxlWb->getModelName(iterId) << std::endl;
      std::cout << "ID      : " << static_cast<unsigned int>(iterId) << std::endl;
      std::cout << "NAME    : " << data->topicBase << std::endl;
      if(data->modeServo == DataServo::ModeServo::JOINT)
      _dxlWb->jointMode(iterId, data->speed, data->acceleration);
      else if(data->modeServo == DataServo::ModeServo::MULTITURN)
        _dxlWb->setMultiTurnControlMode(iterId);
      else
      {
        std::cout << __PRETTY_FUNCTION__ << " error unknown mode " << static_cast<unsigned int>(data->modeServo) << std::endl;
        return false;
      }
      if(!_dxlWb->itemWrite(iterId, "Torque_Enable", 1))
         ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " error torque enable " << iterId);
      ros::Subscriber subsSpeed = _nh.subscribe(data->topicBase + "/speed/des", 1, &DynamixelDevice::callBackSpeed, device.get());
      _subs.push_back(subsSpeed);
      ros::Subscriber subsPos = _nh.subscribe(data->topicBase + "/pos/des", 1, &DynamixelDevice::callBackPosition, device.get());
      _subs.push_back(subsPos);
      _devices.push_back(std::move(device));
      success = true;
    }
    else
      ROS_ERROR("%s - error: No config for servo %i found", __PRETTY_FUNCTION__, id);
    if(++i >= _dxlCnt)
      break;
  }
  return success;
}

bool ServerDynamixel::readServoData(const std::string& filename, std::vector<DataServo>* const dataServo)
{
  tinyxml2::XMLDocument doc;

  doc.LoadFile(filename.c_str());
  tinyxml2::XMLNode* rootNode = doc.FirstChild();

  if (!rootNode)
  {
    ROS_ERROR("no node in xml file.");
    return false;
  }

  for (tinyxml2::XMLNode* node = rootNode->NextSibling()->FirstChild(); node; node = node->NextSibling())
  {
    tinyxml2::XMLElement* element = node->ToElement();

    if(!element)
      continue;

    if(element->Name() == std::string("config"))
    {
      //_bus = new DynamixelBus(node);
      const tinyxml2::XMLElement* element = node->ToElement();

      ROS_INFO("Baudrate %i", element->UnsignedAttribute("baudrate"));
      ROS_INFO("Port %s", element->Attribute("port"));
      ROS_INFO("Protocol version %s\n", element->Attribute("protocol_version"));
      _baudRate = element->UnsignedAttribute("baudrate");
      _device = element->Attribute("port");
      _scanRange = element->UnsignedAttribute("scan_range");
      //ROS_INFO("Scan range %i", element->UnsignedAttribute("scan_range"));
    }
    else if(element->Name() == std::string("servo"))
    {
      const tinyxml2::XMLElement* element = node->ToElement();
      //Dynamixel* device = _bus->createDevice(element);
      ROS_INFO("Load data element for servo ID %i", element->UnsignedAttribute("id"));
      ROS_INFO("--------------------------------------------------------------------------");
      ROS_INFO("-Name of the joint %s" , element->Attribute("name"));
      ROS_INFO("-Basic moving speed %i", element->UnsignedAttribute("speed"));
      ROS_INFO("-Basic acceleration %i", element->UnsignedAttribute("acceleration"));
      ROS_INFO("-Publish position %i",   element->UnsignedAttribute("pubPos"));
      ROS_INFO("-Publish velocity %i",   element->UnsignedAttribute("pubVel"));
      ROS_INFO("-Publish current %i", element->UnsignedAttribute("pubCurrent"));
      ROS_INFO("--------------------------------------------------------------------------\n");

      const uint8_t id            = static_cast<uint8_t>(element->UnsignedAttribute("id"));
      const std::string name      = std::string(element->Attribute("name"));
      const uint16_t speed        = static_cast<uint16_t>(element->UnsignedAttribute("speed"));
      const uint16_t acceleration = static_cast<uint16_t>(element->UnsignedAttribute("acceleration"));
      DynamixelDevice::FlagsPub flagsPub(static_cast<bool>(element->UnsignedAttribute("pubPos")),
                                         static_cast<bool>(element->UnsignedAttribute("pubVel")),
                                         static_cast<bool>(element->UnsignedAttribute("pubCurrent")));
      const uint8_t mode         = static_cast<uint8_t>(element->UnsignedAttribute("mode", 4711));

      std::cout << __PRETTY_FUNCTION__ << " flag ? " << flagsPub.pubCurrent << std::endl;
      DataServo data(id, name, speed, acceleration, flagsPub, static_cast<DataServo::ModeServo>(mode));
      dataServo->push_back(data);
    }
  }
  return true;
}

void ServerDynamixel::updateServos(void)
{
  for(auto& iter : _devices)
    iter->update(*_dxlWb);

}

void ServerDynamixel::timerCallBack(const ros::TimerEvent& event)
{
  if(!_initialized)
  {
    if(this->initDynamixel())
      _initialized = true;
    else
      ROS_ERROR_THROTTLE(1.0, "%s - error. Not initialized yet", __PRETTY_FUNCTION__);
  }
//  ros::Time timer = ros::Time::now();
  this->updateServos();
 // std::cout << __PRETTY_FUNCTION__ << " elapsed time updating servos " << (ros::Time::now() - timer).toSec() << " (s)" << std::endl;
}

} /* namespace ohm_dynamixel */
