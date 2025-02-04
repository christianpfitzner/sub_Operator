/*
 * serial_can.cpp
 *
 *  Created on: 29.10.2015
 *      Author: feesma44884
 */
#include <ohm_can_interface/serial_can.hpp>

SerialCAN::SerialCAN(const std::string& ns, const std::string& serial_port_path, const double& update_frequency_hz,
                     const unsigned int& max_timeout_ms) :
_node_handle(),
_update_timer(),
_serial_port_path(serial_port_path),
_can_subscriber(),
_can_publisher(),
_vcp(),
_vcp_baud_rate(RS232::B_115200),
_max_timeout_ms(max_timeout_ms),
_is_connected(false)
{
    ROS_INFO("ohm_serial_can: starting node");

    //register subscriber for can messages
    _can_subscriber = _node_handle.subscribe<ohm_can_interface::CAN>("/" + ns + "/tx", 100, &SerialCAN::CAN_tx, this);

    //register publisher for can messages
    _can_publisher = _node_handle.advertise<ohm_can_interface::CAN>("/" + ns + "/rx", 100);

    //register update timer
    _update_timer  = _node_handle.createTimer(ros::Duration(1.0 / update_frequency_hz), &SerialCAN::CAN_rx, this);
}

SerialCAN::~SerialCAN() {
    //disconnect
    _vcp.release();
}

bool SerialCAN::connect() {
    //check if connection is already established
    if(_is_connected)   return true;

    //info
    ROS_INFO("ohm_serial_can: Connecting to serial port: %s", (char*)(this->_serial_port_path.c_str()));

    //try to open serial com port
    if(!_vcp.connect(_serial_port_path, _vcp_baud_rate)) {
        ROS_ERROR("ohm_serial_can: Failed to open serial port (%s). Check if you have access rights!", _serial_port_path.c_str());;
        return false;
    }

    ROS_INFO("ohm_serial_can: Cleaning up...");

    //wait 1 sec
    sleep(1);

    //send flush message
    const unsigned char flushData[4] = {0x00, 0x00, '\r', '\n'};
    _vcp.transmitBuffer(flushData, 4);

    //connection established
    _is_connected = true;

    ROS_INFO("ohm_serial_can: Connection established");

    //wait 1 sec
    usleep(1);


    return true;
}

void SerialCAN::run() {
    //check if connection to serial-can-converter is established
    if(!_is_connected) {
        ROS_ERROR("ohm_serial_can: run(): not connected to serial-can-converter! Call connect()!");
        return;
    }

    //start timer
    _update_timer.start();

    //run
    ros::spin();
}


void SerialCAN::CAN_tx(const ohm_can_interface::CAN::ConstPtr& can_data) {
    //buffer for converting data
    unsigned char buffer[16];
    unsigned int  buffer_size = 0;

    //create message
    buffer[0]   =   0x11;   //tell converter to send data
    buffer[1]   =   (unsigned char)(can_data->ID >> 8);
    buffer[2]   =   (unsigned char)(can_data->ID);
    buffer_size  = 3;

    //copy data
    for(unsigned char n = 0; n < can_data->data_size; n++) {
        buffer[buffer_size] = can_data->data[n];
        buffer_size++;
    }

    //write escape sequence
    buffer[buffer_size] = '\r';
    buffer_size++;
    buffer[buffer_size] = '\n';
    buffer_size++;

    //transmit message
    if(!_vcp.transmitBuffer(buffer, buffer_size)) {
        ROS_WARN("ohm_serial_can: Failed to transmit can message");
    }
}

void SerialCAN::CAN_rx(const ros::TimerEvent& event) {
    //buffer for converting data
    ohm_can_interface::CAN can_data;
    unsigned char buffer[16];
    unsigned int  buffer_size = 0;

    //check if there is CAN data
    buffer[0] = 0x44;
    buffer[1] = '\r';
    buffer[2] = '\n';
    buffer_size= 3;

    //send request for download
    if(!_vcp.transmitBuffer(buffer, buffer_size)) {
        ROS_WARN("ohm_serial_can: Failed to transmit download message");
        return;
    }

    //wait for data
    if(_vcp.receiveBuffer(buffer, buffer_size, 16, _max_timeout_ms) != RS232::SUCCESS) {
        ROS_WARN("ohm_serial_can: Failed to receive data!");
        return;
    }

    //check if there is nothing to receive
    if(!(buffer_size == 3 && buffer[0] == 0xff)) {
        //save data
        can_data.ID = (uint16_t)(buffer[0] << 8 | buffer[1]);

        //copy data
        for(unsigned char n = 2; n < (buffer_size - 2); n++) {
            //check overflow
            if(can_data.data_size >= 8) {
                ROS_WARN("ohm_serial_can: Too much data for CAN message!");
                can_data.data_size = 0;
                return;
            }
            else {
                //check if there is data
                //save data
                can_data.data[can_data.data_size] = buffer[n];
                can_data.data_size++;
            }

        }

        //header
        can_data.header.stamp.sec = ros::Time::now().sec;
        can_data.header.stamp.nsec = ros::Time::now().nsec;

        //publish data
        _can_publisher.publish(can_data);
    }
}

int main (int argc, char** argv) {

    std::string serial_port_path;
    double update_frequency_hz;
    int max_timeout_ms;

    //init ros system
    ros::init(argc, argv, "ohm_can_interface");

    //node handle
    ros::NodeHandle nh("~");

    //get namespace
    const std::string ns    =   nh.getNamespace();

    //get parameters
    nh.param<std::string>("serialPortPath", serial_port_path, "/dev/ttyACM0");
    nh.param<double>("updateFrequencyHz", update_frequency_hz, 100.0);
    nh.param<int>("maxTimeoutMS", max_timeout_ms, 10);

    //create instance of serial can converter
    SerialCAN _serial_can_converter(ns, serial_port_path, update_frequency_hz, (unsigned int)(max_timeout_ms));

    //connect
    if(!_serial_can_converter.connect()) {
        return -1;
    }

    //run (calls spin())
    _serial_can_converter.run();


    return 0;
}
