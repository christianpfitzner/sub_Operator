/*
 * canopen_SDO.cpp
 *
 *  Created on: 05.01.2016
 *      Author: feesma44884
 */
#include <ohm_can_interface/canopen.hpp>

namespace CANopen
{
SDO::SDO(const uint16_t node_id, const ACCESS_TYPE access, const uint16_t object_id, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms) :
_can_msg(MSG(COB_SDO_R + (node_id - 1), (uint8_t)(access), (uint8_t)(object_id), (uint8_t)(object_id>>8), multiplexer, (uint8_t)(data), (uint8_t)(data>>8), (uint8_t)(data>>16), (uint8_t)(data>>24))),
_sleep_ms(response_time_ms)
{
}

SDO::SDO(const unsigned int sleep_ms) :
_can_msg(MSG(0, 0)),
_sleep_ms(sleep_ms)
{

}

void SDO::send() {
    CANopen::IManager()->can_tx(_can_msg);
}

bool SDO::check_response(const CANopen::MSG& can_msg) {

    //check can message
    //check response
    //check if Object ID is correct
    if(can_msg._data[1] == _can_msg._data[1] && can_msg._data[2] == can_msg._data[2] && can_msg._data[3] == _can_msg._data[3]) {
        //Object-ID is correct
        //check if SDO contains error code
        if(can_msg._data[0] == 0x80) {
            //extract error code
            uint32_t    error_code  =   (uint32_t)(can_msg._data[7] << 24 | can_msg._data[6] << 16 | can_msg._data[5] << 8 | can_msg._data[4] << 24);

            //throw error
            switch(error_code)
            {
            case 0x05030000:
            {
                ROS_WARN("ohm_can_open: SDO Error: Toggle bit not alternated!");
            }break;
            case 0x05040001:
            {
                ROS_WARN("ohm_can_open: SDO Error: Invalid or unknown client/server command specifier!");
            }break;
            case 0x05040002:
            {
                ROS_WARN("ohm_can_open: SDO Error: Invalid block size!");
            }break;
            case 0x05040003:
            {
                ROS_WARN("ohm_can_open: SDO Error: Invalid sequence number in SDO block upload!");
            }break;
            case 0x05040005:
            {
                ROS_WARN("ohm_can_open: SDO Error: Out of memory!");
            }break;
            case 0x06010000:
            {
                ROS_WARN("ohm_can_open: SDO Error: Unsupported access to an object!");
            }break;
            case 0x06010001:
            {
                ROS_WARN("ohm_can_open: SDO Error: Attempt to read write-only object!");
            }break;
            case 0x06010002:
            {
                ROS_WARN("ohm_can_open: SDO Error: Attempt to write a read-only object!");
            }break;
            case 0x06020000:
            {
                ROS_WARN("ohm_can_open: SDO Error: Object doesn't exist in object dictionary!");
            }break;
            case 0x06040041:
            {
                ROS_WARN("ohm_can_open: SDO Error: Object cannot be mapped to PDO!");
            }break;
            case 0x06040042:
            {
                ROS_WARN("ohm_can_open: SDO Error: Number and length of objects to be mapped exceeds PDO length!");
            }break;
            case 0x06040043:
            {
                ROS_WARN("ohm_can_open: SDO Error: General parameter incompatibility!");
            }break;
            case 0x06060000:
            {
                ROS_WARN("ohm_can_open: SDO Error: Access failed due to hardware error!");
            }break;
            case 0x06070012:
            {
                ROS_WARN("ohm_can_open: SDO Error: Data type does not match, service parameter too long!");
            }break;
            case 0x06090011:
            {
                ROS_WARN("ohm_can_open: SDO Error: Sub-index does not exist!");
            }break;
            case 0x06090030:
            {
                ROS_WARN("ohm_can_open: SDO Error: Value range of parameter exceeded (only for write access)!");
            }break;
            case 0x06090031:
            {
                ROS_WARN("ohm_can_open: SDO Error: Value of parameter written too high!");
            }break;
            case 0x06090032:
            {
                ROS_WARN("ohm_can_open: SDO Error: Value of parameter written too low!");
            }break;
            case 0x06090036:
            {
                ROS_WARN("ohm_can_open: SDO Error: Maximum value is less than minimum value!");
            }break;
            case 0x08000000:
            {
                ROS_WARN("ohm_can_open: SDO Error: General error. When the abort code is 0x08000000 the actual error can be retrieved using the EC command!");
            }break;
            case 0x08000020:
            {
                ROS_WARN("ohm_can_open: SDO Error: Data cannot be transferred to or stored in application!");
            }break;
            case 0x08000022:
            {
                ROS_WARN("ohm_can_open: SDO Error: Data cannot be transferred to or stored in application due to present device state!");
            }break;
            case 0x08000024:
            {
                ROS_WARN("ohm_can_open: SDO Error: There is no valid data available to transmit");
            }break;
            default:
            {
                ROS_WARN("ohm_can_open: SDO Error: Unknown error code received: %08X", error_code);
            }break;
            };

            return false;
        }
        else {
            return true;
        }

    }else
    {
        //error
        ROS_WARN("ohm_can_open: Node responsed with unexpected SDO: Object-ID: %02X %02X %02X | Expected: %02X %02X %02X", can_msg._data[1], can_msg._data[2], can_msg._data[3],
                _can_msg._data[1], _can_msg._data[2], _can_msg._data[3]);

        return false;
    }


    return false;
}

};
