/*
 * canopen_example.cpp
 *
 *  Created on: 17.12.2015
 *      Author: feesma44884
 */
/*
 * main.cpp
 *
 *  Created on: 17.12.2015
 *      Author: feesma44884
 */

#include <ohm_can_interface/canopen.hpp>
#include <ros/ros.h>
#include <signal.h>

#define NUM_NODES 1

ros::Timer  g_timer_show_positions;
ros::Timer  g_timer_move_new_positions;

uint32_t  g_CAN_ID[NUM_NODES]           = {40};         //!< CAN IDs of the nodes
uint32_t  g_position_actual[NUM_NODES]  = {0};          //!< Actual position of the nodes
int32_t   g_velocity_target[NUM_NODES]  = {50000};      //!< Actual velocity of the ndoes

uint64_t  g_BI_set[NUM_NODES]           = {0};
uint64_t  g_BI_get[NUM_NODES]           = {0};


void config_Object_Dictionary(void) {
  //add objects to object dictionary
  CANopen::IOD()->add(0x2012, "Binary interpreter set", 64);
  CANopen::IOD()->add(0x2013, "Binary interpreter get", 64);
  CANopen::IOD()->add(0x3002, "AC", 32);
  CANopen::IOD()->add(0x3020, "BG", 8);
  CANopen::IOD()->add(0x3050, "DC", 32);
  CANopen::IOD()->add(0x3214, "UM", 0);
  CANopen::IOD()->add(0x3146, "MO", 0);
  CANopen::IOD()->add(0x3186, "PA", 32);
  CANopen::IOD()->add(0x31E3, "SP", 32);
}

void init_nodes(void) {
  std::shared_ptr<CANopen::Manager>   manager = CANopen::IManager();

  //add all nodes
  for(auto n = 0; n < NUM_NODES; n++) {
    //add node to manager
    manager->add_node(g_CAN_ID[n]);

    //config PDOs

    //map Transmit PDOs (Node -> Master)
    manager->get_PDO(g_CAN_ID[n], CANopen::COB_PDO_T_1)->register_object("Position actual value", (uint64_t*)(&g_position_actual[n]));
    manager->get_PDO(g_CAN_ID[n], CANopen::COB_PDO_T_1)->config_trigger(CANopen::PDO::ASYNC_TIMER, 1000);
  }
}

bool pre_op_config(void) {
  std::shared_ptr<CANopen::Manager>   manager = CANopen::IManager();

  //bring actuators to operational state
  for(auto n = 0; n < NUM_NODES; n++) {
    //enable PDOs
    manager->get_PDO(g_CAN_ID[n], CANopen::COB_PDO_T_1)->enable();
    manager->get_PDO(g_CAN_ID[n], CANopen::COB_PDO_R_2)->enable();
    manager->get_PDO(g_CAN_ID[n], CANopen::COB_PDO_T_2)->enable();

    //config movement
    manager->get_node(g_CAN_ID[n])->send_SDO(CANopen::SDO::WRITE, "UM", 1, 5);
    manager->get_node(g_CAN_ID[n])->send_SDO(CANopen::SDO::WRITE, "AC", 1, 100000);
    manager->get_node(g_CAN_ID[n])->send_SDO(CANopen::SDO::WRITE, "DC", 1, 100000);
    manager->get_node(g_CAN_ID[n])->send_SDO(CANopen::SDO::WRITE, "SP", 1, g_velocity_target[n]);
/*
    //enable actuator
    manager->get_node(g_CAN_ID[n])->send_SDO(CANopen::SDO::WRITE, "MO", 1, 1);
    manager->get_node(g_CAN_ID[n])->SDO_sleep(5000);*/
  }

  //start functions
  g_timer_show_positions.start();
  g_timer_move_new_positions.start();

  return true;
}

void SigintHandler(int sig)
{
  //info
  ROS_INFO("ohm_can_open: STRG + C: Shutting CANopen Manager down...");

  //stop loop
  g_timer_show_positions.stop();
  g_timer_move_new_positions.stop();

  //release memory
  CANopen::IManager()->release();

  //warning
  ROS_INFO("ohm_can_open: Bye Bye");

  //shutdown
  ros::shutdown();
}

void show_positions(const ros::TimerEvent& event) {
  std::string strOutput = "Positions: ";
  char buffer[16];

  for(auto n = 0; n < NUM_NODES; n++) {
    sprintf(buffer, " Node[%i]: %i", (int)(g_CAN_ID[n]), (int)(g_position_actual[n]));
    strOutput.append(buffer);
  }
  //std::system("clear");
  ROS_INFO("%s", (char*)strOutput.c_str());
}

void send_command(const char* cmd, const uint8_t sub_index, const uint32_t data) {
  //save command



}

void move(const ros::TimerEvent& event) {
  std::shared_ptr<CANopen::Manager>   manager = CANopen::IManager();
}


int main (int argc, char** argv) {
    std::shared_ptr<CANopen::Manager>   manager = CANopen::IManager();


    //init ros system
    ros::init(argc, argv, "ohm_canopen_example");
    ros::NodeHandle node_handle;

    //sigint handler
    signal(SIGINT, SigintHandler);

    //init manager
    manager->init(node_handle, -1.0f);

    //register pre op config function
    manager->register_pre_operational_config_function(pre_op_config);

    //config object dictionary
    config_Object_Dictionary();

    //init nodes
    init_nodes();

    //register timers
    g_timer_show_positions   =   node_handle.createTimer(ros::Duration(1.0 / 5.0), show_positions);
    g_timer_show_positions.stop();

    //move
    g_timer_move_new_positions  = node_handle.createTimer(ros::Duration(10.0), move);
    g_timer_move_new_positions.stop();

    //start program
    ros::spin();

    manager->release();

    return 0;
}
