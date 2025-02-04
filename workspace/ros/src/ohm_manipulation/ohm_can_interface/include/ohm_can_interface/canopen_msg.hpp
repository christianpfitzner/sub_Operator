/*
 * canopen_msg.hpp
 *
 *  Created on: 17.12.2015
 *      Author: feesma44884
 */

#ifndef CANOPEN_MSG_HPP_
#define CANOPEN_MSG_HPP_


/**
 * @struct CANopen::MSG
 *
 * @brief Structure representing a CAN-message with ID and 8 bytes of data
 */
struct CANopen::MSG
{
    /**
     * @brief Constructors
     */
    MSG()                                    :   _ID(0), _data{0, 0, 0, 0, 0, 0, 0, 0}, _data_size(0) {}
    MSG(const uint16_t ID)                   :   _ID(ID), _data{0, 0, 0, 0, 0, 0, 0, 0}, _data_size(0) {}
    MSG(const uint16_t ID, const uint8_t d1) :
        _ID(ID), _data{d1, 0, 0, 0, 0, 0, 0, 0}, _data_size(1) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2) :
        _ID(ID), _data{d1, d2, 0, 0, 0, 0, 0, 0}, _data_size(2) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2, const uint8_t d3) :
        _ID(ID), _data{d1, d2, d3, 0, 0, 0, 0, 0}, _data_size(3) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2, const uint8_t d3, const uint8_t d4) :
        _ID(ID), _data{d1, d2, d3, d4, 0, 0, 0, 0}, _data_size(4) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2, const uint8_t d3, const uint8_t d4, const uint8_t d5) :
        _ID(ID), _data{d1, d2, d3, d4, d5, 0, 0, 0}, _data_size(5) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2, const uint8_t d3, const uint8_t d4, const uint8_t d5, const uint8_t d6) :
        _ID(ID), _data{d1, d2, d3, d4, d5, d6, 0, 0}, _data_size(6) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2, const uint8_t d3, const uint8_t d4, const uint8_t d5, const uint8_t d6, const uint8_t d7) :
        _ID(ID), _data{d1, d2, d3, d4, d5, d6, d7, 0}, _data_size(7) {}
    MSG(const uint16_t ID, const uint8_t d1, const uint8_t d2, const uint8_t d3, const uint8_t d4, const uint8_t d5, const uint8_t d6, const uint8_t d7, const uint8_t d8) :
        _ID(ID), _data{d1, d2, d3, d4, d5, d6, d7, d8}, _data_size(8) {}
    MSG(const ohm_can_interface::CAN& msg) : _ID(msg.ID), _data{msg.data[0], msg.data[1], msg.data[2], msg.data[3], msg.data[4], msg.data[5], msg.data[6], msg.data[7]}, _data_size(msg.data_size)   {}
    MSG(const CANopen::MSG& msg)           : _ID(msg._ID), _data{msg._data[0], msg._data[1], msg._data[2], msg._data[3], msg._data[4], msg._data[5], msg._data[6], msg._data[7]}, _data_size(msg._data_size) {}

    /**
     * @brief Conversions
     */
    operator ohm_can_interface::CAN() const    {ohm_can_interface::CAN msg; msg.ID = _ID; msg.data = {_data[0], _data[1], _data[2], _data[3], _data[4], _data[5], _data[6], _data[7]}; msg.data_size = _data_size; return msg;}

    /**
     * Internal data of CANopen::MSG
     */
    uint16_t  _ID;            //!< ID of the CAN message
    uint8_t   _data[8];       //!< Data of the CAN message: Max 8 byte
    uint8_t   _data_size;     //!< Data size of the CAN message in data array
};

#endif /* CANOPEN_MSG_HPP_ */
