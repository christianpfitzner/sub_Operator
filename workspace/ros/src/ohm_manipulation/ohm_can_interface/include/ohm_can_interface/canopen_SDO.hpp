/*
 * canopen_SDO.hpp
 *
 *  Created on: 05.01.2016
 *      Author: feesma44884
 */

#ifndef CANOPEN_SDO_HPP_
#define CANOPEN_SDO_HPP_

/**
 * @class CANopen::SDO
 *
 * @brief Representing a Service Data Object
 */
class CANopen::SDO
{
public:
    /**
     * @enum ACCESS_TYPE
     *
     * @brief Defines access to the object (read or write)
     */
    enum ACCESS_TYPE
    {
        WRITE    =   0x22,//!< ACCESS_WRITE
        READ     =   0x40 //!< ACCESS_READ
    };

   /**
   * @brief Standard constructor
   */
   SDO() : _can_msg(), _sleep_ms(false)   {}


  /**
   * @brief Constructor generating a SDO
   *
   * @param node_id[in] CANopen Node-ID of the receiver
   * @param access[in] define read or write access
   * @param object_id[in]: ID of the object from the Object Dictionary
   * @param multiplexer[in]: Multiplexer
   * @param data[in] 4 x 8 byte of data
   * @param response_time_ms[in] time to wait for a response
   */
  SDO(const uint16_t node_id, const ACCESS_TYPE access, const uint16_t object_id, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms);

  /**
   * @brief Constructor for generating a sleep SDO
   *
   * This SDO will block sending other SDOs until sleep_ms time has passe (milliseconds) (Does not block the user program)
   *
   * @param sleep_ms[in] Time to sleep in milliseconds
   */
  SDO(const unsigned int sleep_ms);

  /**
   * @brief: Sends message via CAN interface
   */
  void send();

  /**
   * @brief Check if SDO was processed correctly or if an error was sent
   *
   * @param can_msg[in] Received can message
   * @return
   */
  bool check_response(const MSG& can_msg);


  const unsigned int  _sleep_ms;       //!< If this variable is greater than 0 the SDO is a SLEEP SDO. No message will be send until time in milliseconds has passed

private:

  const MSG   _can_msg;             //!< Message send via CAN interface

  friend class Node;
};

#endif /* CANOPEN_SDO_HPP_ */
