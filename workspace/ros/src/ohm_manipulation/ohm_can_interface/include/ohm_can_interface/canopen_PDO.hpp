/*
 * canopen_PDO.hpp
 *
 *  Created on: 11.01.2016
 *      Author: feesma44884
 */

#ifndef CANOPEN_PDO_HPP_
#define CANOPEN_PDO_HPP_

/**
 *  @class CANopen::PDO
 *
 *  @brief Class representing a PDO (Process Data Object)
 *  For more Information see DS301 Reference Guide p.46
 *  http://elmomc-germany.de/filefolder/Manuals%20%28CANopen%29/Gold/MAN-G-DS301.pdf
 */
class CANopen::PDO
{
public:
  /**
   * @enum TYPE
   */
  enum TYPE {
    PDO_TRANSMIT  = 0,//!< PDO_TRANSMIT: Transmit PDO
    PDO_RECEIVE   = 1 //!< PDO_RECEIVE: Receive PDO
  };

  /**
   * @enum TRIGGER
   */
  enum TRIGGER {
    SYNC_ACYCLIC  = 0,//!< SYNC_ACYCLIC: Triggered every time sync is received
    SYNC_CYCLIC   = 1,//!< SYNC_CYCLIC: Triggered every nth time sync is received
    ASYNC_TIMER   = 2 //!< ASYNC_TIMER: Triggered async by timer
  };

  /**
   * @brief Constructs PDO object
   *
   * @param cob_id[in] ID of the PDO and type (PDO1, PDO2, PDO3 | Transmit or Receive)
   * @param node[in] Reference to the node
   */
  PDO(const COB cob_id, Node& node);

  /**
   * @brief Registers an object to the mapping object list
   *
   * @param object[in] Object from the object dictionary
   * @param data[in] Ptr to the data
   *
   * @return true if there is enough space to map object | false if there is not enough space
   */
  bool  register_object(const OD::Object& object, uint64_t* data);
  bool  register_object(const std::string& object_name, uint64_t* data);
  bool  register_object(const uint16_t object_index, uint64_t* data);

  /**
   * @brief Configures the trigger event
   *
   * @param trigger[in] See description of enum TRIGGER
   * @param option[in] EVERY_SYNC: ignored SYNC: option defines the number of sync signals received to trigger event ASYNC_TIMER: defines time in milliseconds when event is triggered
   * @param send_RPDO_automatically[in] If true the RPDO data is sent automatically | false you have to send it with send_data()
   * @return
   */
  void  config_trigger(const TRIGGER trigger, const uint16_t option, const bool send_RPDO_automatically = true);

  /**
   * @brief Enables the PDO
   */
  void  enable(void);

  /**
   * @brief Disables the PDO
   */
  void  disable(void);

  /**
   * @brief Sends R-PDO data to the node
   */
  void send_data(void);

private:

  /**
   * @brief Sends the configuration to the node
   *        Called until set up process by CANopen::Manager
   */
  void  send_config(void);

  /**
   * @brief Processes the received data
   *
   * @param can_data[in] Received PDO data (called from Node)
   */
  void  process_data(const MSG& can_data);

  /**
   * @brief Clears the PDO
   */
  void  clear_mapped_objects(void);

  /**
   * Maps a object in a PDO the position inside the PDO is defined by the id
   *
   * @param id[in] ID of the object inside the PDO (id = 1 -> first object mapped to PDO...)
   * @param object[in] Object from object dictionary
   */
  void map_object(const uint8_t id, const OD::Object& object);

  /**
   * Register the mapped objects (from map_object) to the PDO
   *
   * @param num_objects[in] Number of mapped objects
   */
  void register_mapped_objects(const uint8_t num_objects);

  /**
   * @brief PDO is processed immediatly after even
   */
  void config_sync_acyclic(void);

  /**
   * @brief PDO is processed after receiving num_sync_signals number of sync signals
   *
   * @param num_sync_signals[in] Number of sync signals to receive before process PDO
   */
  void config_sync_cyclic(const uint8_t num_sync_signals);

  /**
   * @brief PDO is processed after timer_milliseconds have ellapsed
   *
   * @param timer_milliseconds[in] Timer in milliseconds
   */
  void config_sync_asynchronous(const uint16_t timer_milliseconds);

  Node&                     _node;                        //!< Reference to node
  const COB                 _cob_id;                      //!< COB (TPDO1, RPDO1, TPDO2, ...)
  const uint8_t             _id;                          //!< ID of the PDO (PDO 0 - 3)
  const TYPE                _type;                        //!< Type of the PDO (Transmit/Receive)

  uint16_t                  _index_config_general;        //!< Index (in OD) of general config
  uint16_t                  _index_config_mapping;        //!< Index (in OD) of mapping config

  TRIGGER                   _trigger_config;              //!< Trigger event
  uint16_t                  _trigger_option;              //!< Trigger event option

  std::vector<OD::Object*>  _mapped_object_list;          //!< List saving all mapped objects
  std::vector<uint64_t*>    _mapped_object_variable_list; //!< List saving the ptr to the data

  bool                      _config_send;                 //!< Saves true if configuration is send
  bool                      _is_enabled;                  //!< Saves true if PDO is enabled
  bool                      _send_automatically;          //!< Saves true if R-PDO is send automatically

  friend CANopen::Node;
};

#endif /* CANOPEN_PDO_HPP_ */
