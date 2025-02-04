/*
 * serial_can.h
 *
 *  Created on: 29.10.2015
 *      Author: feesma44884
 */

#ifndef SERIAL_CAN_HPP_
#define SERIAL_CAN_HPP_

/**
 * @brief Standard includes
 */
#include <ros/ros.h>

/**
 * @brief Project includes
 */
#include <ohm_can_interface/serial_can_definitions.h>
#include <ohm_can_interface/CAN.h>
#include <ohm_can_interface/serial_rs232.hpp>

/**
 * @class SerialCAN
 * @brief Does communication ros <-> serial <-> CAN
 */
class SerialCAN
{
public:
    /**
     * Constructor of serial-can-converter
     *
     * @param ns[in] Namespace of node
     * @param serial_port_path[in] Path to the serial port
     * @param update_frequency_hz[in] Update frequency in Hz
     * @param max_timeout_ms[in] Maximum time to wait for an answer of the serial converter board (arduino uno)
     */
    SerialCAN(const std::string& ns, const std::string& serial_port_path, const double& update_frequency_hz,
              const unsigned int& max_timeout_ms);

    /**
     * Destructor: Releases memory
     */
    ~SerialCAN();

    /**
     * Connecting serial-can-converter (Arduino UNO via VCP)
     *
     * @return true if connection is successfully established
     */
    bool connect();

    /**
     *  Runs the serial-can-converter
     */
    void run();

private:


    /**
     * Sends a CAN message: ROS -> CAN
     *
     * @param can_data[in] can data which will be sent to the CAN network
     */
    void CAN_tx(const ohm_can_interface::CAN::ConstPtr& can_data);

    /**
     * Check if serial-can-converter received a CAN message: CAN -> ROS
     *
     * @param event[in] Time
     */
    void CAN_rx(const ros::TimerEvent& event);



    ros::NodeHandle           _node_handle;         /***< Instance of ROS node */
    ros::Timer                _update_timer;        /***< Timer for updating */
    const std::string         _serial_port_path;    /***< Path to serial device */

    ros::Subscriber            _can_subscriber;     /***< Subscriber for CAN messages */
    ros::Publisher             _can_publisher;      /***< Publisher for CAN messages */

    RS232::Port                _vcp;                /***< Instance of the virtual com port */
    const RS232::BaudRate      _vcp_baud_rate;      /***< Baud rate of the viertual com port */

    const unsigned int         _max_timeout_ms;     /***< Maximum timout time before abort receiving */
    bool                       _is_connected;       /***< True if the VCP is connected to application */
};

#endif /* SERIAL_CAN_HPP_ */
