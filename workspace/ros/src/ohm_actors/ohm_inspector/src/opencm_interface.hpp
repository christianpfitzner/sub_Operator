/*
 * opencm_interface.hpp
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_SRC_OPENCM_INTERFACE_HPP_
#define OHM_INSPECTOR_SRC_OPENCM_INTERFACE_HPP_

#include <ros/ros.h>

#include "serial_rs232.hpp"

/**
 * Definitions for the OpenCM Interface
 */
#define SI_MAX_BUFFER_SIZE  32
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
 * Enumeration used to define state of the inspector
 */
enum InspectorState {
    STATE_PARKED    =   0, //!< The inspector is in park position -> no movement is possible
    STATE_RELEASED  =   1, //!< The inspector is in relased state -> movement is possible
    STATE_MOVE_HOME =   2, //!< The inspector is moving to home position
    STATE_MOVE_PARK =   3  //!< The inspector is moving to park position
};

/**
 * OpenCMInterface class is a communication Interface to the OpenCM9.04 board
 */
class OpenCMInterface
{
public:

    /**
     * Constructor of the OpenCM Interface
     *
     * @param timeoutMS[in] Maximum time no communication can happen before disconnecting from OpenCM board
     * @param deviceName[in] Name of the serial device attached to the OpenCM board
     * @param baudRate[in] Communication speed in baud (standard B_115200)
     */
    OpenCMInterface(const uint16_t& timeoutMS, const std::string& deviceName, const RS232::BaudRate& baudRate = RS232::B_19200);

    /**
     * Connects to the OpenCM board and setups the communication parameters (timeoutMS).
     * After a successful connection data has to be send to the OpenCM board before timeoutMS exceed or it will disconnect
     *
     * @param errorID[out]: If a failure occured because of the dynamixel drives the id of the drive will be saved in this var
     *
     * @return true when connection was established successfully
     */
    bool connect(unsigned int& errorID);

    /**
     * Closes the connection to the OpenCM board
     */
    void disconnect();

    /**
     * Check if data was sent to the OpenCM board.
     * If not it will send a ping signal to avoid disconnection. Should be called every run in message loop.
     *
     * @return true when connection is working
     */
    bool keepConnection();

    /**
     * Sends a ping to the OpenCM board.
     * This function is called from keepConnection() if no message was sent in a run.
     *
     * @return true if ping was sent successfully
     */
    bool sendPingMessage();

    /**
     * Sends command to stop every movement.
     *
     * @return true if stop message was sent successfully
     */
    bool sendStopMessage();

    /**
     * Sends message to move to home position.
     *
     * @return true if command was sent successfully (no feedback if home position is reached!)
     */
    bool sendMoveHomeMessage();

    /**
     * Sends message to move to park position
     *
     * @return true if command was sent successfully (no feedback if park position is reached!)
     */
    bool sendMoveParkMessage();

    /**
     * Sends command to move on a straight line.
     * Send command to move on a straight line with the speed FWD/BWD and UP/DOWN for positioning and A and B for changing the orientation.
     *
     * @param speed_X[in] Speed in FWD/BWD direction (X)
     * @param speed_Y[in] Speed in UP/Down direction (Y)
     * @param speed_A[in] Orientation A
     * @param speed_B[in] Orientation B
     *
     * @return true when command was send successfully
     */
    bool sendMoveCPMessage(const float& speed_X, const float& speed_Y, const float& speed_A, const float& speed_B);

    /**
     * Sends request for the current angle positions and receives the result.
     *
     * @param angles[out] Array with the angle positions
     * @param state[out] Contains the current state of the inspector
     *
     * @return true when command was send successfully
     */
    bool getCurrentAngles(float* angles, InspectorState& state);

    /**
     * Sends command to reset dynamixels via relai
     *
     * @return true if message was sent successfully
     */
    bool sendDynamixelResetMessage();

    /**
     * Returns the name of the serial device connected to the OpenCM board
     *
     * @return std::string with the device name
     */
    const std::string& getDeviceName()              {return _deviceName;}

    /**
     * Returns the maximum Time no connection can appear before disconnecting
     *
     * @return max timeout time in milliseconds
     */
    const uint16_t& getTimeoutMS()                  {return _timeoutMS;}


private:
    /**
     * Sends a data packet to the OpenCM board.
     * A data packet consists out of a Message ID, additional data, checksum and an escape sequence.
     *
     * @param msg[in] Message ID
     * @param data[in] Additional array of data
     * @param dataSize[in] Size of the data array
     *
     * @return true when data was sent successfully
     */
    bool sendDataPacket(const uint8_t& msg, uint8_t* data = NULL, const uint8_t& dataSize = 0);

    /**
     * Connects to the OpenCM board.
     * Sends a request for secure connection to the OpenCM board and setups timeout time
     *
     * @param timeoutMS[in] Max time no communication can happen, before the board and/or host disconnects
     * @param errorID[out]: If a failure occured because of the dynamixel drives the id of the drive will be saved in this var
     *
     * @return true if connect message was sent successfully
     */
    bool sendConnectMessage(const uint16_t& timeoutMS, unsigned int& errorID);

    /**
     * Waits for the confirm message of the OpenCM board
     * After sending a message to the OpenCM board its answering with the message ID when the
     * message was received successfully.
     *
     * @param msg[in]: Message which was sent to the OpenCM board
     *
     * @return true if confirmation was received successfully
     */
    bool waitForConfirm(const uint8_t& msg);

    /**
     * Checks if the checksum is valid
     *
     * @param data[in] Array with the data of the message
     * @param dataSize[in] Size of the array
     *
     * @return true if checksum is valid
     */
    bool checkChecksum(unsigned char* data, const unsigned int& dataSize);

    const uint16_t      _timeoutMS;     /**< Maximum time no communication can happen before disconnecting */
    RS232::Port         _vcp;           /**< Instance of a virtual com port */
    const std::string   _deviceName;    /**< Name of the serial port the OpenCM board is attached to */
    RS232::BaudRate     _baudRate;      /**< Communication speed in baud */

    uint8_t             _pingErrorCnt;  /**< Saves how much errors occured since the last valid communication had happened */
    bool                _messageSent;   /**< Saves true if a message was sent -> needed for keepConnection */
};





#endif /* OHM_INSPECTOR_SRC_OPENCM_INTERFACE_HPP_ */
