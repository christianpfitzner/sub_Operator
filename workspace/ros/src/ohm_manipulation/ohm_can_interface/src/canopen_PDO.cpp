/*
 * canopen_PDO.cpp
 *
 *  Created on: 11.01.2016
 *      Author: feesma44884
 */
#include <ohm_can_interface/canopen.hpp>

namespace CANopen
{


PDO::PDO(const COB cob_id, Node& node) :
_node(node),
_cob_id(cob_id),
_id(((cob_id/128)-3)/2),
_type(static_cast<PDO::TYPE>(((cob_id/128)-3)%2)),
_index_config_general(0),
_index_config_mapping(0),
_trigger_config(PDO::SYNC_ACYCLIC),
_trigger_option(0),
_mapped_object_list(),
_mapped_object_variable_list(),
_config_send(false),
_is_enabled(false),
_send_automatically(true)
{
  //check type
  switch(_type) {
  case PDO_TRANSMIT:
  {
    _index_config_general = 0x1800 + _id;
    _index_config_mapping = 0x1A00 + _id;
  }break;
  case PDO_RECEIVE:
  {
    _index_config_general = 0x1400 + _id;
    _index_config_mapping = 0x1600 + _id;
  }break;
  }

  //clear lists
  _mapped_object_list.clear();
  _mapped_object_variable_list.clear();
}

bool PDO::register_object(const OD::Object& object, uint64_t* data) {
  uint8_t   free_size = 64; //free size of 64 bit

  //check size of the object
  if(object._size == 0) {
    ROS_ERROR("ohm_can_open: PDO::register_object: Object is not mappable to a PDO");
  }

  //check objects in list
  for(auto it : _mapped_object_list) {
    //decrease size
    free_size -=  it->_size;
  }

  //check if there is enough space left to map object
  if(free_size >= object._size) {
    //map object
    _mapped_object_list.push_back((OD::Object*)(&object));
    _mapped_object_variable_list.push_back(data);
    return true;
  }

  //no configuration is done until it's send
  _config_send  = false;

  return false;
}

bool PDO::register_object(const std::string& object_name, uint64_t* data) {
  //get object
  const OD::Object&   object  = IOD()->get(object_name);

  //check for error
  if(object._index == 0xffff)  return false;

  return register_object(object, data);
}

bool PDO::register_object(const uint16_t object_index, uint64_t* data) {
  //get object
  const OD::Object&   object  = IOD()->get(object_index);

  //check for error
  if(object._index == 0xffff)  return false;

  return register_object(object, data);
}

void PDO::config_trigger(const TRIGGER trigger, const uint16_t option, const bool send_RPDO_automatically) {
  _trigger_config = trigger;
  _trigger_option = option;
  _send_automatically = send_RPDO_automatically;
}

void PDO::enable(void) {
  //check if configuration is done
  if(!_config_send) {
    ROS_ERROR("ohm_can_open: PDO::enable(): Abort sending enable signal! Configuration is not send yet!");
    return;
  }

  //compute enable code
  uint32_t  enable_code = (uint32_t)(_cob_id + (_node.get_node_id() - 1));

  //send SDO
  _node.send_SDO(SDO::WRITE, _index_config_general, 1, enable_code);

  //is enabled
  _is_enabled = true;
}

void PDO::disable(void) {
  //compute enable code
  uint32_t  disable_code = (uint32_t)(0x80000000)  | (uint32_t)((uint16_t)(_cob_id) + (_node.get_node_id() - 1));

  //send SDO
  _node.send_SDO(SDO::WRITE, _index_config_general, 1, disable_code);

  //is disabled
  _is_enabled = false;
}

void PDO::send_data(void) {
  static  unsigned int sync_cnt = 0;

  //check if is enabled -> if not abort
  if(!_is_enabled || _mapped_object_list.size() == 0)  return;

  //check if sync is needed
  if(_trigger_config == SYNC_CYCLIC && _send_automatically == true) {
    if(sync_cnt < _trigger_option) {
      sync_cnt++;
      return;
    }
    else {
      sync_cnt  = 0;
    }
  }

  uint8_t   object_id       = 0;
  uint8_t   can_data_pos    = 0;
  uint8_t   can_data[8]     = {0, 0, 0, 0, 0, 0, 0, 0};

  for(auto it : _mapped_object_list) {
    //check data size
    uint8_t   object_size_byte  = it->_size / 8;
    uint64_t  var = (*_mapped_object_variable_list[object_id++]);

    //save can data to variable
    for(auto n = 0; n < object_size_byte; n++) {
      can_data[can_data_pos++]  = (uint8_t)(var >> 8 * (n));
    }
  }

  //send data
  IManager()->can_tx(MSG((static_cast<uint16_t>(_cob_id) + _node.get_node_id() - 1),
                       can_data[0], can_data[1], can_data[2], can_data[3], can_data[4],
                       can_data[5], can_data[6], can_data[7]));
}

void PDO::send_config(void) {

  //disable PDO
  disable();

  //clear mapped objects
  clear_mapped_objects();

  //check if there are objects to map
  if(_mapped_object_list.size() == 0) return;

  uint8_t id = 1;
  for(auto it : _mapped_object_list) {
    map_object(id++, (*it));
  }

  //register number of mapped objects
  register_mapped_objects(_mapped_object_list.size());

  //check sync configuration
  switch(_trigger_config) {
  case  SYNC_ACYCLIC:
  {
    config_sync_acyclic();
  }break;
  case SYNC_CYCLIC:
  {
    config_sync_cyclic((uint8_t)_trigger_option);
  }break;
  case ASYNC_TIMER:
  {
    config_sync_asynchronous(_trigger_option);
  }break;
  }

  //configuration is send
  _config_send  = true;
}

void PDO::process_data(const MSG& can_data) {
  //check if is enabled -> if not abort
  if(!_is_enabled)  return;

  uint8_t object_id  = 0;
  uint8_t can_data_pos    = 0;

  for(auto it : _mapped_object_list) {
    //check data size
    uint8_t   object_size_byte  = it->_size / 8;
    uint64_t  var = 0;

    //save can data to variable
    for(auto n = 0; n < object_size_byte; n++) {
      var |= (uint64_t)(can_data._data[can_data_pos++] << 8 *(n));
    }

    //save to variable
    (*_mapped_object_variable_list[object_id++])  = var;
  }
}

void PDO::clear_mapped_objects(void) {
  //clear mapping
  _node.send_SDO(SDO::WRITE, _index_config_mapping, 0, 0);
}

void PDO::map_object(const uint8_t id, const OD::Object& object) {
  _node.send_SDO(SDO::WRITE, _index_config_mapping, id, (object._index << 16) | (uint32_t)(object._size));
}

void PDO::register_mapped_objects(const uint8_t num_objects) {
  _node.send_SDO(SDO::WRITE, _index_config_mapping, 0, num_objects);
}

void PDO::config_sync_acyclic(void) {
  //generate acyclic syncronisation
  _node.send_SDO(SDO::WRITE, _index_config_general, 2, 0);
}

void PDO::config_sync_cyclic(const uint8_t num_sync_signals) {
  //generate acyclic syncronisation
  _node.send_SDO(SDO::WRITE, _index_config_general, 2, num_sync_signals);
}

void PDO::config_sync_asynchronous(const uint16_t timer_milliseconds) {
  //generate acyclic syncronisation
  _node.send_SDO(SDO::WRITE, _index_config_general, 2, 254);

  //setup event timer
  _node.send_SDO(SDO::WRITE, _index_config_general, 5, (uint32_t)(timer_milliseconds));
}
}
