/*
 * canopen_example_sim.cpp
 *
 *  Created on: 21.12.2015
 *      Author: feesma44884
 */
#include <ros/ros.h>
#include <ohm_can_interface/CAN.h>
#include <ohm_can_interface/canopen.hpp>

bool firstCall = true;
const uint8_t        g_CAN_ID    =   40;
ros::Publisher       g_CAN_pub;
ros::Subscriber      g_CAN_sub;

void can_receiver(const ohm_can_interface::CAN::ConstPtr& can_data) {
    CANopen::MSG    can_msg(*can_data);

    //check message
    if(can_msg._ID == 0x0000) {
        //check command
        if(can_msg._data[0] == 0x81) {
            //reset network received -> answer with boot up message
            sleep(5);
            g_CAN_pub.publish((ohm_can_interface::CAN)CANopen::MSG(0x700 + g_CAN_ID));
            firstCall  = true;
        }
        else if(can_msg._data[0] == 0x01) {
            //reset network received -> answer with boot up message
            g_CAN_pub.publish((ohm_can_interface::CAN)CANopen::MSG(CANopen::COB_PDO_T_1 + g_CAN_ID - 1));
        }
    }
    else {
        if(can_msg._ID >= CANopen::COB_EMERGENCY && can_msg._ID <= CANopen::COB_END) {
            //check message
            const uint8_t            can_id      =   can_msg._ID % OHM_CANOPEN_COB_SIZE;
            const CANopen::COB       cob_type    =   static_cast<CANopen::COB>(can_msg._ID - (can_id - 1));

            switch(cob_type)
            {
            case CANopen::COB_PDO_R_1:
            {

            }break;
            case CANopen::COB_PDO_R_2:
            {

            }break;
            case CANopen::COB_PDO_R_3:
            {

            }break;
            case CANopen::COB_PDO_R_4:
            {

            }break;
            case CANopen::COB_SDO_R:
            {
                g_CAN_pub.publish((ohm_can_interface::CAN)(CANopen::MSG((uint16_t)(CANopen::COB_SDO_T + g_CAN_ID - 1), 0x60, can_msg._data[1], can_msg._data[2], can_msg._data[3])));
            }break;
            }
        }
    }
}

int main (int argc, char** argv) {

    const CANopen::MSG    can_messages[1] =   {CANopen::MSG(0x0701)};


    //init ros system
    ros::init(argc, argv, "ohm_canopen_example_sim");
    ros::NodeHandle node_handle;

    //register publisher
    g_CAN_pub   =   node_handle.advertise<ohm_can_interface::CAN>("can_interface/rx", 10);
    g_CAN_sub   =   node_handle.subscribe<ohm_can_interface::CAN>("can_interface/tx", 10, can_receiver);

    //wait
    sleep(1);

    //update frequency
    ros::Rate   loop_rate(100);

    unsigned int  CNT       = 0;
    uint32_t      data      = 0;

    while(ros::ok())
    {
        ros::spinOnce();
        loop_rate.sleep();

        if(firstCall == true) {
          if(CNT++ > 1000) {
            firstCall = false;
            CNT = 0;
          }
        }
        else {
          if(CNT++ > 2) {
            CNT = 0;
            uint32_t  data2 = data + 10;
            g_CAN_pub.publish((ohm_can_interface::CAN)(CANopen::MSG((uint16_t)(CANopen::COB_PDO_T_1 + g_CAN_ID - 1),
                (uint8_t)(data), (uint8_t)(data>>8), (uint8_t)(data>>16), (uint8_t)(data>>24),
                (uint8_t)(data2), (uint8_t)(data2>>8), (uint8_t)(data2>>16), (uint8_t)(data2>>24))));
            data++;
          }
        }

    }

    return 0;
}
