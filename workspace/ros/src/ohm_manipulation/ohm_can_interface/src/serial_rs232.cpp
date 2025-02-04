/*
 * serial_rs232.cpp
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */
#include <ohm_can_interface/serial_rs232.hpp>

using namespace boost::asio;

namespace RS232
{

Port::Port() :
_ioService(NULL), _serialPort(NULL), _deadlineTimer(NULL),
_isInitialized(false)
{
    for(unsigned int n = 0; n < RS232_SERIAL_ASYNC_MAX_BUFFER_SIZE; n++) {
        this->_asyncReceiveBuffer[n] = 0x00;
    }
    this->_asyncReceiveBufferSize = 0;
}

Port::~Port() {
    //check if device was released
    if(_isInitialized == true) {
        //no -> release device
        release();
    }
}

Result Port::connect(const std::string& deviceName, const BaudRate& baudRate) {

    //check if class is already connected
    if(_isInitialized == true)    release();

    //allocate memory
    if(_ioService == NULL)      _ioService      =   new boost::asio::io_service;
    if(_serialPort == NULL)     _serialPort     =   new boost::asio::serial_port(*_ioService);
    if(_deadlineTimer == NULL)  _deadlineTimer  =   new boost::asio::deadline_timer(*_ioService);

    //memory is now allocated
    _isInitialized   =   true;

    //try to open the port
    try {
        //Open the serial port
        _serialPort->open(deviceName);

        //config baudrate
        unsigned int nbaudRate = 0;

        switch(baudRate) {
        case B_9600:
            nbaudRate = 9600;
            break;
        case B_19200:
            nbaudRate = 19200;
            break;
        case B_38400:
            nbaudRate = 38400;
            break;
        case B_57600:
            nbaudRate = 57600;
            break;
        case B_115200:
            nbaudRate = 115200;
            break;
        }

        //set baudrate
        _serialPort->set_option(serial_port_base::baud_rate(nbaudRate));

        //set flow control
        serial_port_base::flow_control flowCtrl(serial_port_base::flow_control::none);
        _serialPort->set_option(flowCtrl);

        //set one stop bit
        serial_port_base::stop_bits stopBit(serial_port_base::stop_bits::one);
        serial_port_base::parity parityBit(serial_port_base::parity::none);

        //RS232 8N1
        _serialPort->set_option(serial_port_base::character_size(8)); //8 data bits
        _serialPort->set_option(stopBit); //1 stop bit
        _serialPort->set_option(parityBit); //0 parity bits

    }
    catch(boost::system::system_error& error) {

        //throw error message
        ROS_ERROR("RS232::Port::connect(): boost error: %s", error.what());

        return FAILED;
    }


    return SUCCESS;
}

void Port::release() {
    //check if is initialized
    if(_isInitialized == false) return;

    //close port
    if(_serialPort != NULL) {
        if(_serialPort->is_open()) {
            _serialPort->close();
        }
    }

    //free memory
    SAFE_DELETE(_deadlineTimer);
    SAFE_DELETE(_serialPort);
    SAFE_DELETE(_ioService);

    //connection closed
    _isInitialized    =   false;
}

Result Port::transmitBuffer(const unsigned char data[], const unsigned int& dataSize) {
    //check if class is initialized
    if(!_isInitialized) {ROS_ERROR("RS232::Port::transmitBuffer(): Not initialized!"); return ERROR_NOT_INITIALIZED;}

    try {
        //transmit data
        _serialPort->write_some(buffer(data, dataSize));
    }
    catch(boost::system::system_error& error) {
        //throw error
        ROS_ERROR("RS232::Port::transmitBuffer(): boost error: %s", error.what());

        return FAILED;
    }

    return SUCCESS;
}

Result Port::receiveBuffer(unsigned char* data, unsigned int& dataSize, const unsigned int& dataMaxSize, const unsigned int& timeoutMS) {
    //check if class is initialized
    if(!_isInitialized) {ROS_ERROR("RS232::Port::transmitBuffer(): Not initialized!"); return ERROR_NOT_INITIALIZED;}

    Result          result;
    unsigned char   buffer  =   0;
    unsigned int    pos     =   0;

    //dataSize = 0
    dataSize = 0;

    //receive data until escape sequence is reached or maximum amount of data is reached
    while(pos < dataMaxSize) {

        //receive data
        result = receiveChar(buffer, timeoutMS);

        //check if error occured
        if(result != SUCCESS) {
            return result;
        }

        //save data
        data[pos++]   =   buffer;

        //check for escape sequence
        if(pos > 2) {
            if(data[pos - 2] == '\r' && data[pos - 1] == '\n') {
                dataSize = pos;

                //receiving finished
                return SUCCESS;
            }
        }

    }

    //throw error
    ROS_INFO("data: %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",
            data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11],
            data[12], data[13], data[14], data[15]);
    ROS_ERROR("RS232::Port::receiveBuffer(): Max receive buffer size reached!");

    return ERROR_MAX_SIZE;
}

Result Port::receiveChar(unsigned char& data, const unsigned int& timeoutMS) {

    unsigned char receiveBuffer[1];
    bool data_available = false;

    //read data asynchronous
    _serialPort->async_read_some( boost::asio::buffer(receiveBuffer),
                                  boost::bind(&Port::read_callback,
                                  this,
                                  boost::ref(data_available),
                                  boost::ref(*_deadlineTimer),
                                  boost::asio::placeholders::error,
                                  boost::asio::placeholders::bytes_transferred)
                                );

    //config timeout
    boost::posix_time::millisec delay_boost(timeoutMS);
    _deadlineTimer->expires_from_now(delay_boost);
    _deadlineTimer->async_wait(boost::bind(&Port::wait_callback,
                               this,
                               boost::ref(*_serialPort),
                               boost::asio::placeholders::error));

    //block until async callback is finished or timeout occured
    _ioService->run();

    //reset
    _ioService->reset();

    //check if data was received
    if(!data_available) {
        //there was no data received
        ROS_WARN("RS232::Port::receiveChar(): Timeout!");

        return ERROR_TIMEOUT;
    }

    //save received data
    data = receiveBuffer[0];

    return SUCCESS;
}

void Port::read_callback(bool& data_available, boost::asio::deadline_timer& timeout,
                         const boost::system::error_code& error, std::size_t bytes_transferred )
{
    //std::cout << "debug: called -> SerialUSB::read_callback(...)" << std::endl;
    if (error || !bytes_transferred)
    {
        // No data was read!
        data_available = false;
        return;
    }

    timeout.cancel();  // will cause wait_callback to fire with an error
    data_available = true;
}

void Port::wait_callback(boost::asio::serial_port& ser_port, const boost::system::error_code& error)
{
    //std::cout << "debug: called -> SerialUSB::wait_callback(...)" << std::endl;
    if(error)
    {
        // Data was read and this timeout was canceled
        return;
    }

    ser_port.cancel();  // will cause read_callback to fire with an error
}

} /*namespace RS232*/
