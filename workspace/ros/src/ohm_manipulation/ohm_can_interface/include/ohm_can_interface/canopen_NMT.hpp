/*
 * canopen_NMT.hpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */

#ifndef CANOPEN_NMT_HPP_
#define CANOPEN_NMT_HPP_

/**
 * @class CANopen::NMT
 *
 * @brief CANopen network management class -> used in CANopen::Manager()
 */
class CANopen::NMT
{
private:
    /**
     * @enum CMD
     *
     * @brief Enumeration of network managemenet commands
     */
    enum CMD {
        CMD_START_NODE          =   0x01,//!< CMD_START_NODE
        CMD_STOP_NODE           =   0x02,//!< CMD_STOP_NODE
        CMD_SET_PRE_OP_MODE     =   0x80,//!< CMD_SET_PRE_OP_MODE
        CMD_RESET_NODE          =   0x81,//!< CMD_RESET_NODE
        CMD_RESET_COMMUNICATION =   0x82 //!< CMD_RESET_COMMUNICATION
    };

    /**
     * @brief Sends a network management command via the CANopen network
     *
     * @param cmd[in] Command to send to the adressed node (0 = all nodes)
     * @param can_id[in] CAN ID of the addressed node
     *
     */
    void    send(const CMD cmd, const uint8_t can_id = 0);

    friend class CANopen::Manager;
};

#endif /* CANOPEN_NMT_HPP_ */
