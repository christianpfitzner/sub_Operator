/*
 * canopen_OD.cpp
 *
 *  Created on: 27.01.2016
 *      Author: feesma44884
 */
#include <ohm_can_interface/canopen.hpp>

namespace CANopen
{
OD::OD(void)
{
  //register standard objects
  add(0x0000, "sleep", 0);
  add(0x1400, "RPDO parameter", 0);
  add(0x1600, "RPDO mapping parameter", 0);
  add(0x1800, "TPDO parameter", 0);
  add(0x1A00, "TPDO mapping parameter", 0);
  add(0x6040, "Control word", 16);
  add(0x6041, "Status word", 16);
  add(0x6060, "Modes of operation", 8);
  add(0x6064, "Position actual value", 32);
  add(0x606C, "Velocity actual value", 32);
  add(0x6077, "Torque actual value", 16);
  add(0x607A, "Target position", 32);
  add(0x6081, "Profile velocity", 32);
  add(0x6083, "Profile acceleration", 32);
  add(0x6084, "Profile deceleration", 32);
  add(0xffff, "object not found", 0xff);
}

OD::~OD(void) {
  _object_dictionary.clear();
}

bool OD::add(const uint16_t index, const std::string& name, const uint8_t size) {
  //check if entry exists
  if(_object_dictionary.find(index) == _object_dictionary.end()) {
    //add entry
    _object_dictionary.insert(std::pair<uint16_t, Object>(index, Object(index, name, size)));

    return true;
  }

  ROS_WARN("ohm_can_open: OD::append() Index does already exist!");
  return false;
}

const OD::Object& OD::get(const uint16_t index) {
  //search for object
  auto  it  = _object_dictionary.find(index);

  //check if object exist
  if(it != _object_dictionary.end()) {
    return (it->second);
  }

  //Print warning
  ROS_WARN("ohm_can_open: OD::get(): No object found in object dictionary! Index: %04X", index);

  //get error object
  it = _object_dictionary.find(0xffff);

  return it->second;
}

const OD::Object& OD::get(const std::string& name) {
  //search for name
  for(auto it = _object_dictionary.begin(); it != _object_dictionary.end(); it++) {
    //search for name
    if(!strcmp(name.c_str(), it->second._name.c_str())) {
      return it->second;
    }
  }

  //Print warning
  ROS_WARN("ohm_can_open: OD::get(): No object found in object dictionary! Name: %s", name.c_str());

  //get error object
  auto it = _object_dictionary.find(0xffff);

  return it->second;
}
};

