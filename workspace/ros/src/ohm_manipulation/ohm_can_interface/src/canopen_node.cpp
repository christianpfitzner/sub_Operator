/*
 * canopen_node.cpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */
#include <ohm_can_interface/canopen.hpp>

namespace CANopen
{

Node::Node(const uint8_t node_id) :
_node_id(node_id),
_node_state(NS_RESET),
_PDO_list(),
_SDO_queue(),
_SDO_timeout_cnt(0),
_SDO_max_timeout_cnt(0),
_is_registered(false)
{
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_T_1, PDO(COB_PDO_T_1, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_R_1, PDO(COB_PDO_R_1, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_T_2, PDO(COB_PDO_T_2, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_R_2, PDO(COB_PDO_R_2, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_T_3, PDO(COB_PDO_T_3, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_R_3, PDO(COB_PDO_R_3, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_T_4, PDO(COB_PDO_T_4, *this)));
  _PDO_list.insert(std::pair<COB, PDO>(COB_PDO_R_4, PDO(COB_PDO_R_4, *this)));
}


void Node::update(void) {
    //check if there are SDO messages to send
    if(!_SDO_queue.empty()) {
      //check if new message can be send
      if(_SDO_timeout_cnt == 0) {
        //check message type (only normal messages are send | sleep is not send)
        if(_SDO_queue.front()._can_msg._ID != 0) {
          //send message
          _SDO_queue.front().send();
        }

        //calculate timeout time
        _SDO_max_timeout_cnt  = (unsigned int)(ceil(_SDO_queue.front()._sleep_ms * IManager()->get_update_frequency_Hz() * 0.001));
      }

      //check for timeout
      if(_SDO_timeout_cnt > _SDO_max_timeout_cnt) {
          _SDO_timeout_cnt    =   0;

          //check timeout type
          if(_SDO_queue.front()._can_msg._ID == 0) {
            _SDO_queue.pop();
          }
          else {
            ROS_WARN("ohm_can_open: Node %i: No SDO response received! Sending SDO again!", _node_id);
          }

      }
      else {
        //increase cnt
        _SDO_timeout_cnt++;
      }
    }
    else {
        _SDO_timeout_cnt = 0;
    }
}

void Node::send_SDO(const SDO::ACCESS_TYPE access, const uint16_t object_id, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms) {
  _SDO_queue.push(SDO(_node_id, access, object_id, multiplexer, data, response_time_ms));
}

void Node::send_SDO(const SDO::ACCESS_TYPE access, const OD::Object& object, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms) {
  _SDO_queue.push(SDO(_node_id, access, object._index, multiplexer, data, response_time_ms));
}

void Node::send_SDO(const SDO::ACCESS_TYPE access, const std::string& object_name, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms) {
  //get object
  const OD::Object&   object  = IOD()->get(object_name);

  //check for error
  if(object._index == 0xffff)  {
    ROS_ERROR("ohm_can_open: Node::send_SDO(): Object '%s' does not exist!", object_name.c_str());
    return;
  }

  _SDO_queue.push(SDO(_node_id, access, object._index, multiplexer, data, response_time_ms));
}

void Node::SDO_sleep(const unsigned int timeout_ms) {
  _SDO_queue.push(SDO(timeout_ms));
}

const uint8_t Node::get_node_id(void) {
  return _node_id;
}

PDO* const Node::get_PDO(const COB cob_id) {
  //search for COB
  auto it = _PDO_list.find(cob_id);

  //check if object was found
  if(it != _PDO_list.end()) {
    return &it->second;
  }

  ROS_ERROR("ohm_can_open: Node::get_PDO() PDO not found!");
  return nullptr;
}

bool Node::process_data(const COB cob_type, const MSG& can_data) {
    //check for PDO
    if(cob_type >= CANopen::COB_PDO_T_1 && cob_type < CANopen::COB_SDO_T) {
        //check for T PDOs
        switch(cob_type) {
        case COB_PDO_T_1: {_PDO_list.find(COB_PDO_T_1)->second.process_data(can_data);}break;
        case COB_PDO_T_2: {_PDO_list.find(COB_PDO_T_2)->second.process_data(can_data);}break;
        case COB_PDO_T_3: {_PDO_list.find(COB_PDO_T_3)->second.process_data(can_data);}break;
        case COB_PDO_T_4: {_PDO_list.find(COB_PDO_T_3)->second.process_data(can_data);}break;
        }

    }
    //check for SDO
    else if(cob_type >= CANopen::COB_SDO_T && cob_type < CANopen::COB_SDO_R)
    {
        //transmit SDO from node
        //check response
        if(!_SDO_queue.empty())   {
            if(_SDO_queue.front().check_response(can_data)) {
                //remove SDO
                _SDO_queue.pop();
                _SDO_timeout_cnt = 0;
            }
            else {
                //remove SDO
                _SDO_queue.pop();
                _SDO_timeout_cnt = 0;

                //error received
                ROS_ERROR("ohm_can_open: Node: %i: SDO response error!", _node_id);
                return false;
            }
        }
    }
    else if(cob_type >= CANopen::COB_EMERGENCY && cob_type < (CANopen::COB_EMERGENCY + 0x80)) {
        //emergency from node
    }
    else {
        //error from node
    }

    return true;
}

void Node::send_PDO_config(void) {
  //send config of all nodes
  for(auto& it : _PDO_list) {
    it.second.send_config();
  }
}

void Node::SYNC(void) {
  //update R-PDOs
  if(_PDO_list.find(COB_PDO_R_1)->second._send_automatically) _PDO_list.find(COB_PDO_R_1)->second.send_data();
  if(_PDO_list.find(COB_PDO_R_2)->second._send_automatically) _PDO_list.find(COB_PDO_R_2)->second.send_data();
  if(_PDO_list.find(COB_PDO_R_3)->second._send_automatically) _PDO_list.find(COB_PDO_R_3)->second.send_data();
  if(_PDO_list.find(COB_PDO_R_4)->second._send_automatically) _PDO_list.find(COB_PDO_R_4)->second.send_data();
}

};
