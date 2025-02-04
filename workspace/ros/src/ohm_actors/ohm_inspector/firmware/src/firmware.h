/*
 * firmware.h
 *
 *  Created on: 03.02.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_FIRMWARE_FIRMWARE_H_
#define OHM_INSPECTOR_FIRMWARE_FIRMWARE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "definitions.h"
#include "core/inc/iwdg.h"
#include "core/inc/wirish.h"
#include "core/inc/Dynamixel.h"
#include "serial_interface.h"
#include "dynamixel_manager.h"
#include "inspector.h"
#include "InspectorMath.h"

/**
 * Enumeration used to define current state of connection
 */
enum FirmwareConnectionStates {
    STATE_WAIT_FOR_DYNAMIXELS   =   0,//!< OpenCM board waits until all dynamixels are connected
    STATE_WAIT_FOR_HOST         =   1,//!< OpenCM board waits until host establishs connection
    STATE_CONNECTED             =   2 //!< OpenCM board is connected to dynamixels and host -> application runs
};

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
 * Enumeration used to save the commands received from host
 */
enum InspectorCMD {
    CMD_NONE        =   0,//!< No command received
    CMD_PARK        =   1,//!< Move the inspector to park position
    CMD_HOME        =   2,//!< Move the inspector to home position -> release from park position
    CMD_MOVE        =   3 //!< Move the inspector with continous path
};

/**
 * Main Class of the Firmware.
 */
class Firmware
{
public:

    /**
     * Constructor: Creates class
     */
    Firmware();

    /**
     * Initializes the OpenCM board.
     * Configures Pin of the board and attaches the interrupts.
     */
    void init();

    /**
     * Main message loop.
     * Runs the firmware infinite. Check dynamixel drive state and connection state.
     */
    void messageLoop();

    /**
     * Releases all allocated memory.
     */
    void release();

private:
    /**
     * Runs the application when connected to dynamixels and hosts
     */
    void run();

    /**
     * Register the drives in the dynamixel manager
     */
    void setupServoDrives();

    /**
     * Tries to enable connection to dynamixel drives
     *
     * @return true if connection was established, false if not
     */
    bool connectDynamixels();

    /**
     * Waits until host establishes connection to OpenCM board
     *
     * @return true if connection was established to host
     */
    bool waitForHost();

    /**
     * Checks if everything is connected and no exception happened
     *
     * @return true if everything is connected
     */
     bool checkConnectionState();

     /**
      * Check if the inspector is in park position
      *
      * @return true if inspector is in park position
      */
     bool checkIfInspectorIsParked();

     /**
      * Check if the inspector is in home position
      *
      * @return true if inspector is in home position
      */
     bool checkIfInspectorIsHome();

     /**
      * Sets the state of the inspector depending on its position
      * Changes the state to PARKED if inspector is in parking position
      * Changes the state to RELEASED if inspector is not in parking position
      */
     void checkInspectorState();

    /**
     * Process the received data from the serial port.
     * Get the received data and calls the related command.
     */
    void processReceivedData();

    /**
     * Refuses the request to connect the board and sends back an error code
     */
    void refuseConnectionRequest();

    /**
     * Resets the Timeout timer.
     * Resets the timer which disconnects if timeout time is exceeded.
     */
    void resetTimeout();

    /**
     * Checks if a timeout event occurred.
     *
     * @return true when timeout time exceeded -> disconnect
     */
    bool checkTimeout();

    /**
     * Resets the timer
     */
    inline void resetTimer()    {_timerStamp = millis();}


    SerialInterface             _serialInterface;   /**< Instance of the communication Interface*/
    DynamixelManager::Manager   _dynamixelManager;  /**<Instance of the dynamixel managment class*/
    Inspector::Control          _inspectorControl;  /**<Instance of the inspector control class*/

    FirmwareConnectionStates    _connectionState;   /**<Saves the current connection state*/
    InspectorState              _inspectorState;    /**<Saves the current state of the inspector*/
    InspectorCMD                _inspectorCMD;      /**<Saves the commands received from host*/

    long                        _timerStamp;        /**<Saves time stamp, when timer was started*/
    uint16                      _timerMS;           /**<Saves how much time has passed since timer stamp was set*/

    long                        _timeoutStamp;      /**<Saves the last time stamp*/
    uint16                      _timeoutMS;         /**<Saves the timeout value */

    float                       _cp_Speeds[4];      /**<Saves the speed values received from host*/

friend void serial_callbackfunc(byte* data, byte dataSize);
friend void timer1_callbackfunc();
};

extern Firmware g_firmware;

#endif /* OHM_INSPECTOR_FIRMWARE_FIRMWARE_H_ */
