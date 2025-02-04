/*
 * canopen_NMT.cpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */
#include <ohm_can_interface/canopen.hpp>

void CANopen::NMT::send(enum CMD cmd, const uint8_t can_id) {
    const MSG can_msg(static_cast<uint16_t>(COB_NMT), static_cast<uint8_t>(cmd), can_id);
    CANopen::IManager()->can_tx(can_msg);
}
