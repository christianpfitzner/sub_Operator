/*
 * canopen_manager.cpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */
#include <ohm_can_interface/canopen.hpp>

std::shared_ptr<CANopen::Manager> const CANopen::IManager(void) {
    return Manager::instance();
}

CANopen::OD* const CANopen::IOD(void) {
  return Manager::instance()->get_OD();
}

void CANopen::debug_show_message(const char* topic, const MSG& msg) {
  std::string output  =   topic;
  char buffer[16];

  output.append(": CAN: ");
  sprintf(buffer, "%04X ", msg._ID);
  output.append(buffer);

  for(unsigned int n = 0; n < 8; n++) {
      if(n < msg._data_size) {
          sprintf(buffer, "| %02X ", msg._data[n]);
      }
      else {
          sprintf(buffer, "| 00 ");
      }
      output.append(buffer);
  }

  ROS_INFO("%s", output.c_str());
}

namespace CANopen
{

  Manager::~Manager(void) {
    //disable debug mode
    _debug_mode = false;

    //stop timers
    _update_timer.stop();
    _sync_timer.stop();

    //release memory
    for(auto& node : _node_list) {
      if(node.second) {
        node.second.reset();
      }
    }
  }


  void Manager::init(ros::NodeHandle& node_handle, const double sync_frequency_hz, const bool reset, const double update_frequency_hz) {
      //set variables
      _debug_mode             =   false;
      _sub_state              =   0;
      _update_frequency_hz    =   update_frequency_hz;
      _sync_frequency_hz      =   sync_frequency_hz;
      _reset                  =   reset;

      //reset std::vectors
      _node_list.clear();

      //config update timer
      _update_timer   =   node_handle.createTimer(ros::Duration(1.0 / _update_frequency_hz), &CANopen::Manager::update, this);

      //check if sync timer is needed
      if(_sync_frequency_hz > 0.0f) {
        _sync_timer     =   node_handle.createTimer(ros::Duration(1.0 / _sync_frequency_hz), &CANopen::Manager::SYNC, this);
        _sync_timer.stop();
      }


      //register publisher and subscriber
      _can_publisher  =   node_handle.advertise<ohm_can_interface::CAN>("can_interface/tx", 100);
      _can_subscriber =   node_handle.subscribe<ohm_can_interface::CAN>("can_interface/rx", 100, &CANopen::Manager::can_rx, this);

      //check if everything is fine
      if(!_can_publisher) {
          ROS_ERROR("ohm_can_open: init(): Failed to register CAN publisher!");
          return;
      }
      if(!_can_subscriber) {
          ROS_ERROR("ohm_can_open: init(): Failed to register CAN subscriber!");
          return;
      }

      //information
      ROS_INFO("ohm_can_open: Initializing ohm-can-open manager...");
  }

  void Manager::add_node(const uint8_t node_id) {
    //info
    ROS_INFO("ohm_can_open: Adding node to node list (CAN-ID: %i | Hex: %02X)", node_id, node_id);

    //check if node already exist
    auto it = _node_list.find(node_id);

    //register new node
    if(it == _node_list.end()) {
      //save node in std::map
      _node_list.insert(std::pair<uint8_t, std::shared_ptr<Node>>(node_id, std::shared_ptr<Node>(new Node(node_id))));
      return;
    }
    //error
    ROS_WARN("ohm_can_open: Manager::add_node(): Node with CAN-ID: %i already exists!", node_id);
  }

  void Manager::register_pre_operational_config_function(bool (*pre_op_config_func)(void)) {
     _pre_op_user_config  = pre_op_config_func;
  }

  void Manager::can_tx(const MSG& can_msg) {
      //check debug mode
      if(_debug_mode) debug_show_message("ohm_can_open: CANopen::Manager::can_tx:", can_msg);

      //can message
      ohm_can_interface::CAN message = static_cast<ohm_can_interface::CAN>(can_msg);

      //header
      message.header.stamp       = ros::Time::now();
      message.header.frame_id    = "ohm_canopen_manager";

      //send message
      _can_publisher.publish(message);
  }

  const Manager::State& Manager::get_state(void) {
    return _manager_state;
  }

  std::shared_ptr<Node> const Manager::get_node(const uint8_t node_id) {
    //search for CAN ID
    auto it = _node_list.find(node_id);

    //check if node exist
    if(it != _node_list.end()) {
      return it->second;
    }

    //error
    ROS_ERROR("ohm_can_open: Manager::node(): No node with CAN-ID: %i found! Functions returns nullptr!", node_id);
    return nullptr;
  }

  PDO* const Manager::get_PDO(const uint8_t node_id, const COB& cob_id) {
    return get_node(node_id)->get_PDO(cob_id);
  }

  OD* const Manager::get_OD(void) {
    return &_object_dictionary;
  }

  const double& Manager::get_update_frequency_Hz(void) {
    return _update_frequency_hz;
  }

  Manager::Manager(void) :
  _object_dictionary(),
  _network_management(),
  _manager_state(MNS_START),
  _update_timer(),
  _sync_timer(),
  _can_subscriber(),
  _can_publisher(),
  _update_frequency_hz(1000.0),
  _sync_frequency_hz(-1.0),
  _sub_state(0),
  _node_list(),
  _num_nodes_registered(0),
  _enable_operational_mode(false),
  _debug_mode(false),
  _reset(false),
  _pre_op_user_config(nullptr)
  {
  }

  void Manager::update(const ros::TimerEvent& event) {
      static bool next_state =   false;

      //update nodes
      if(_manager_state != MNS_START) {
        for(auto& it : _node_list) {
          it.second->update();
        }
      }

      //switch state
      switch(_manager_state) {
      case MNS_START:
      {
          //start CANopen network
          next_state  =   start();
      }break;
      case MNS_CONFIG:
      {
          //configure CANopen nodes
          next_state = config();

      }break;
      case MNS_PRE_OPERATIONAL:
      {
        //pre op
        next_state  = pre_operational();
      }break;
      case MNS_OPERATIONAL:
      {
        //SYNC
      }break;
      };

      //check if next state can be applied
      if(next_state == true) {
          //reset vars
          _sub_state  =   0;

          //set next state
          switch(_manager_state) {
          case MNS_START:
          {
            next_state = false;
            _manager_state=MNS_CONFIG;
            ROS_INFO("ohm_can_open: Switching to Config state...");
          }break;
          case MNS_CONFIG:
          {
            next_state = false;
            _manager_state=MNS_PRE_OPERATIONAL;
            ROS_INFO("ohm_can_open: Switching to Pre-Operational state...");
          }break;
          case MNS_PRE_OPERATIONAL:
          {
            ROS_INFO("ohm_can_open: Switching to Operational state...");
            next_state = false;
            _manager_state=MNS_OPERATIONAL;

            //start SYNC timer
            if(_sync_frequency_hz > 0.0f) _sync_timer.start();
          }break;
          }
      }

  }

  void Manager::can_rx(const ohm_can_interface::CAN::ConstPtr& can_data) {
      //convert data
      const MSG can_msg((*can_data));

      //check debug mode
      if(_debug_mode) debug_show_message("ohm_can_open: CANopen::Manager::can_rx:", can_msg);

      //check for object types with CAN ID
      if(can_msg._ID >= CANopen::COB_EMERGENCY && can_msg._ID <= CANopen::COB_END) {
          //extract CANopen Node-ID of the node
          const uint8_t   node_id     =   can_msg._ID % OHM_CANOPEN_COB_SIZE;
          const COB       cob_type    =   static_cast<COB>(can_msg._ID - (node_id - 1));

          //search in list for Node-ID
          auto node = _node_list.find(node_id);

          //check system state
          if(_manager_state != MNS_START) {
            //process received data
            if(!node->second->process_data(cob_type, can_msg)) {
                ROS_ERROR("ohm_can_open: Failed to process CAN message (CAN-ID: %i, COB-Type: %i)", node_id, cob_type);
            }
          }
          else{
             //check if node exists in list
             if(node != _node_list.end()) {
               if(cob_type == COB_ERROR || cob_type == COB_SDO_T) {
                 //check if node is registered
                 if(!node->second->_is_registered) {
                   ROS_INFO("ohm_can_open: CANopen node registered to the network (CAN-ID: %i | Hex: %02X)", node_id, node_id);
                   node->second->_is_registered = true;
                   _num_nodes_registered++;
                 }

               }
             }
             else {
               ROS_WARN("ohm_can_open: New CANopen node try to register to the network! Network not in MNS_START mode! (CAN-ID: %i | Hex: %02X)", node_id, node_id);
             }
          }
      }
      //check for other COB type
      else if(can_msg._ID == CANopen::COB_NMT) {

      }
      else if(can_msg._ID == CANopen::COB_TIME_STAMP) {

      }

      //COB SYNC is ignored, because it should be never received from master!

  }

  bool Manager::start(void)
  {
      static uint32_t  time_stamp = 0;

      //reset network
      switch(_sub_state)
      {
      case 0:
      {
          //information
          ROS_INFO("ohm_can_open: Starting CANopen network...");

          //stop
          _network_management.send(NMT::CMD_STOP_NODE);

          //wait
          _update_timer.setPeriod(ros::Duration(1.0));

          //next state -> wait
          _sub_state++;
      }break;
      case 1:
      {
          //check if network has to be resetted
          if(_reset) {
            //reset network
            ROS_INFO("ohm_can_open: Reset Network...");

            //send reset network message to all nodes
            _network_management.send(NMT::CMD_RESET_NODE);
            _network_management.send(NMT::CMD_START_NODE);
          }
          else {
            ROS_INFO("ohm_can_open: Start nodes...");

            //start nodes
            _network_management.send(NMT::CMD_START_NODE);
          }

          //wait for nodes
          _update_timer.setPeriod(ros::Duration(1.0 / _update_frequency_hz));

          //next state
          _sub_state++;
      }break;
      case 2:
      {
          //wait for controllers
          ROS_INFO("ohm_can_open: Waiting for %i nodes to register at network...", (unsigned int)_node_list.size());

          //save time stamp
          time_stamp  = ros::Time::now().sec;

          //next state
          _sub_state++;
      }break;
      case 3:
      {
          //check if all nodes have registered
          if(_num_nodes_registered >= _node_list.size()) {

              //all nodes have registered
              ROS_INFO("ohm_can_open: All nodes have registered");

              //next sub state
              _sub_state++;
          }
          else if((ros::Time::now().sec - time_stamp) >= 2) {

            //send SDO to every controller to check if it exists
            for(auto& it : _node_list) {
              //check if node has registered
              if(it.second->_is_registered) continue;

              uint16_t  ID  = static_cast<uint16_t>(COB::COB_SDO_R) + (it.second->_node_id - 1);
              can_tx(MSG(ID, 0x40, 0x61, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00));
              //ROS_INFO("SEND SDO %i", it.second->node_id);
            }

            //save time stamp
            time_stamp  = ros::Time::now().sec;
          }
      }break;
      case 4:
      {
          //start nodes
          ROS_INFO("ohm_can_open: Starting all nodes...");

          //start all CANopen node
          _network_management.send(NMT::CMD_START_NODE);

          //wait
          _update_timer.setPeriod(ros::Duration(5.0));

          ROS_INFO("ohm_can_open: Waiting...");

          //next sub state
          _sub_state++;
      }break;
      case 5:
      {
          //wait
          _update_timer.setPeriod(ros::Duration(1.0 / _update_frequency_hz));

          return true;
      }break;
      }

      return false;
  }

  bool Manager::config(void) {

    //reset network
    switch(_sub_state)
    {
    case 0:
    {
        //information
        ROS_INFO("ohm_can_open: Starting config nodes...");

        _sub_state++;
    }break;
    case 1:
    {
        //set update frequency
        _update_timer.setPeriod(ros::Duration(1.0 / _update_frequency_hz));

        //sending PDO config
        ROS_INFO("ohm_can_open: Sending PDO configuration to nodes...");

        for(auto& it : _node_list) {
          it.second->send_PDO_config();
        }

        _sub_state++;
    }break;
    case 2:
    {
        bool  all_SDO_send  = true;

        //wait until all SDOs are send
        for(auto& it : _node_list) {
          if(!it.second->_SDO_queue.empty())  all_SDO_send  = false;
        }


        if(all_SDO_send == true)  _sub_state++;
    }break;
    case 3:
    {
        //configuration finished
        ROS_INFO("ohm_can_open: Configuration finished...");

        return true;
    }break;
    };

    return false;
  }

  bool Manager::pre_operational(void) {
    switch(_sub_state) {
    case 0:
    {
      bool  result =  true;

      //check if user config was defined
      if(_pre_op_user_config != nullptr) {
        result  = _pre_op_user_config();
      }
      else {
        ROS_WARN("ohm_can_open: Manager::update(): No user pre operational function registred! No PDO will be enabled or actuators will go to operational state by default!");
        result  = true;
      }

      //check for error
      if(result == false) {
        ROS_ERROR("ohm_can_open: Manager::update(): Error in user pre operational config function!");
        _update_timer.setPeriod(ros::Duration(2.0));
      }
      else {
        _update_timer.setPeriod(ros::Duration(1.0 / _update_frequency_hz));
        _sub_state++;
      }
    }break;
    case 1:
    {
      bool  all_SDO_send  = true;

      //wait until all SDOs are send
      for(auto& it : _node_list) {
        if(!it.second->_SDO_queue.empty())  all_SDO_send  = false;
      }


      if(all_SDO_send == true)  return true;
    }break;
    }

    return false;
  }

  void Manager::SYNC(const ros::TimerEvent& event) {
    for(auto& it : _node_list) {
      it.second->SYNC();
    }

    can_tx(MSG(COB_SYNC));
  }

};
