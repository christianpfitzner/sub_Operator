/*
 * serial_interface.hpp
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_FIRMWARE_SRC_SERIAL_INTERFACE_H_
#define OHM_INSPECTOR_FIRMWARE_SRC_SERIAL_INTERFACE_H_

#include "core/inc/wirish.h"

/**
 * Definitions for serial interface
 */
#define SI_MAX_BUFFER_SIZE  VCOM_RX_BUFLEN
#define SI_MIN_MSG_SIZE     4

/**
 * @brief: Definitions for communication
 *
 * Communication Protocol works like this
 *
 * |  Byte1  | Byte2 .. Byte n-3 | Byte n-2 | Byte n-1 | Byte n |
 * | Message | Data              | Checksum | \r       | \n     |
 *
 */
#define SI_MSG_PING             0x20    ///< Data: None
#define SI_MSG_CONNECT          0x21    ///< Data: uint16 timeout (2 x bytes)
#define SI_MSG_STOP             0x30    ///< Data: None
#define SI_MSG_MOVE_HOME        0x31    ///< Data: None
#define SI_MSG_MOVE_PARK        0x32    ///< Data: None
#define SI_MSG_MOVE_CP          0x33    ///< Data: float speed_X(4 x bytes) | float speed_Y(4 x bytes) | float speed_A(4 x bytes) | float speed_B(4 x bytes)
#define SI_MSG_GET_STATE        0x34    ///< Data: float angle_1(4 x bytes) | float angle_2(4 x bytes) | float angle_3(4 x bytes) | float angle_4 (4x bytes) | uint8 inspectorState (1 byte)
#define SI_MSG_ERROR            0xf0    ///< Data: ID of dynamixel drive with error (1 x bytes)
#define SI_MSG_RESET_DYNAMIXEL  0x4f    ///< Data: None

/**
 * Class which manages the communication between OpenCM board and a host connected via the virtal com port.
 */
class SerialInterface
{
public:

    /**
     * Constructor
     */
    SerialInterface();

    /**
     * Sends a data packet to the host
     *
     * @param msg[in] Message ID
     * @param data[in] (Optional) Array with data
     * @param dataSize[in] (Optional) Size of the array
     */
    void sendDataPacket(const uint8& msg, uint8* data = NULL, const uint8 dataSize = 0);

    /**
     * Informs the class that the data buffer was read and can be overwritten by a new message.
     */
    void dataBufferRead()       {_dataBufferRead = true;}

    /**
     * Tells if new data is available.
     *
     * @return true if new data is available
     */
    bool newDataAvailable()     {if(_dataBufferRead == false) return true; return false;}

    /**
     * Set the state of the OpenCM board to connected
     */
    void connected()            {_isConnected = true;}

    /**
     * Sets the state of the OpenCM board to disconnected
     */
    void disconnected()         {_isConnected = false;}

    /**
     * Returns the connection state
     *
     * @return true if is connected
     */
    const volatile bool& isConnected()   {return _isConnected;}

    /**
     * Variables to read received data
     */
    volatile uint8      _dataBuffer[SI_MAX_BUFFER_SIZE];        /**<Saves the received data for processing*/
    volatile uint8      _dataBufferSize;                        /**<Number of bytes in data buffer*/

private:

    /**
     * Callback function -> assigned to the serial port of the OpenCM board
     *
     * @param data[in] Array with the received data
     * @param dataSize[in] Size of the array
     */
    void callback_dataReceived(byte* data, byte dataSize);

    volatile uint8      _sendBuffer[SI_MAX_BUFFER_SIZE];        /**<Buffer for sending data*/
    volatile uint8      _sendBufferSize;                        /**<Size of the send buffer*/

    volatile uint8      _receiveBuffer[SI_MAX_BUFFER_SIZE];     /**<Buffer for received data*/
    volatile uint8      _receiveBufferSize;                     /**<Number of received bytes*/
    volatile bool       _receiveInProgress;                     /**<Saves true as long as receiving data*/

    volatile bool       _dataBufferRead;                        /**<Saves true when data buffer was read*/

    volatile bool       _isConnected;                           /**<Saves true when a connection is established*/


friend void serial_callbackfunc(byte* data, byte dataSize);
};

#endif /* OHM_INSPECTOR_FIRMWARE_SRC_SERIAL_INTERFACE_H_ */
