/*
 * serial_rs232.hpp
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */

#ifndef SERIAL_RS232_HPP_
#define SERIAL_RS232_HPP_

/**
 * @brief Definitions
 */
#define RS232_SERIAL_ASYNC_MAX_BUFFER_SIZE 64

#include <ros/ros.h>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>

/**
 * Namespace used for RS232 components
 */
namespace RS232 {
    /**
     * Enumeration of BaudRates
     */
    enum BaudRate   {B_9600     =   0, //!< Baud Rate: 9600
                     B_19200    =   1, //!< Baud Rate: 19200
                     B_38400    =   2, //!< Baud Rate: 38400
                     B_57600    =   3, //!< Baud Rate: 57600
                     B_115200   =   4};//!< Baud Rate: 115200

    /**
     * Enumeration of Results
     */
    enum Result     {FAILED = 0,               //!< Failure
                     SUCCESS = 1,              //!< Success
                     ERROR_NOT_INITIALIZED = 2,//!< Class is not initialized
                     ERROR_TIMEOUT = 3,        //!< Timeout happened
                     ERROR_MAX_SIZE = 4};      //!< Maximum array size exceeded

/**
 * Interface to a serial port of the PC.
 */
class Port
{
public:
    /**
     * Creates the class
     */
     Port();

    /**
     * Desctructor.
     * Releases closes the connection and releases all allocated memory.
     */
    ~Port();

    /**
     * Initializes the connection via the RS232 Port
     *
     * @param deviceName[in] Name of the serial port
     * @param baudRate[in] Communication speed
     *
     * @return SUCCESS when connection was established successfully
     */
    Result connect(const std::string& deviceName, const BaudRate& baudRate);

    /**
     * Closes the port and releases all allocated memory
     */
    void release();

    /**
     * Transmits the data buffer via RS232 protocol
     *
     * @param data[in] Array containing the data to send
     * @param dataSize[in] Size of the array
     *
     * @return SUCCESS if data was sent successfully
     */
    Result transmitBuffer(const unsigned char data[], const unsigned int& dataSize);

    /**
     * Receives data via the RS232 protocol.
     * Receiving is finished when escape sequence was received "\r\n"
     *
     * @param data[out] Array with the received data
     * @param dataSize[out] Size of the array
     * @param dataMaxSize[in] Maximum size of the array
     * @param timeoutMS[in] Maximum time to wait for a byte to receive
     *
     * @return SUCCESS if data was received successfully
     */
    Result receiveBuffer(unsigned char* data, unsigned int& dataSize, const unsigned int& dataMaxSize, const unsigned int& timeoutMS);



private:

    /**
     * Receives one byte/char
     * Quits when timeout time is exceeded
     *
     * @param data[out] Byte/Char received
     * @param timeoutMS[in] Maximum time in milliseconds to wait for data
     *
     * @return Success if data was received successfully
     */
    Result receiveChar(unsigned char& data, const unsigned int& timeoutMS);

    /**
     * Called from boost
     *
     * @param data_available
     * @param timeout
     * @param error
     * @param bytes_transferred
     */
    void read_callback(bool& data_available, boost::asio::deadline_timer& timeout,
                       const boost::system::error_code& error, std::size_t bytes_transferred );

    /**
     * Called from boost
     *
     * @param ser_port
     * @param error
     */
    void wait_callback(boost::asio::serial_port& ser_port, const boost::system::error_code& error);


    boost::asio::io_service*        _ioService;                                                 /**< Instance of boost I/O service */
    boost::asio::serial_port*       _serialPort;                                                /**< Instance of boost serial port */
    boost::asio::deadline_timer*    _deadlineTimer;                                             /**< Instance of boost timer */

    unsigned char                   _asyncReceiveBuffer[RS232_SERIAL_ASYNC_MAX_BUFFER_SIZE];    /**< Buffer for async receiving data */
    unsigned int                    _asyncReceiveBufferSize;                                    /**< Saves the size of the received data */

    bool                            _isInitialized;         /**< Saves true when class is initialized */
};

}
#endif /* SERIAL_RS232_HPP_ */
