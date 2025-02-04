/*
 * inspector_node.hpp
 *
 *  Created on: 19.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_SRC_INSPECTOR_NODE_HPP_
#define OHM_INSPECTOR_SRC_INSPECTOR_NODE_HPP_

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <ohm_actors_msgs/Command.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <sstream>
#include "definitions.h"
#include "opencm_interface.hpp"

/**
 * InspectorNode manages the communication between the OpenCM Board and the Host PC.
 */
class InspectorNode
{
public:
    /**
     * Constructor for the InspectorNode class.
     *
     * @param topicName[in] Name of the command messages
     * @param serialPort[in] Name of the serial port the OpenCM9.04 is mounted to.
     * @param timeoutMS[in] When timeoutMS milliseconds no communication was done the OpenCM board disconnects.
     * @param maxErrorCnt[in]: Maximum count of errors which can happen before disconnecting.
     * @param updateFrequencyHz[in] Update frequency of the node in Hz.
     * @param speed2DScale[in] Scale factor of the 2D Speed
     * @param speedAxisScale[in] Scale Factor for the speed of the axis for orientation
     *
     */
    InspectorNode(const std::string& topicName, const std::string& serialPort, const uint16_t& timeoutMS,
                  const uint8_t& maxErrorCnt, const double& updateFrequencyHz,
                  const float& speed2DScale, const float& speedAxisScale);


    /**
     * Message Loop of the applications.
     * Runs the node as long as the user shut's it down. Even if a connection error occurred the node tries to
     * reconnect until the user cancels the node.
     *
     */
    void runNode();

private:

    /**
     * States of communication.
     */
    enum CommunicationState {
        CS_SEND_COMMAND_DATA      =   0,//!< Interface is sending command data to OpenCM board
        CS_RECEIVE_ANGLE_DATA     =   1 //!< Interface is receiving current angles of the drives from OpenCM board
    };

    /**
     * Connects to the OpenCM9.04 board.
     *
     * @return true when connection is established.
     */
    bool connect();

    /**
     * Main Function for the node. The application runs here.
     *
     * @return False if an error occurred.
     */
    bool messageLoop();

    /**
     * Processes the received messages.
     * Process the received messages and checks for how long no data was sent. If there was no data for timeoutMS
     * all data is set to 0.
     *
     */
    void processMessages();

    /**
     * Sends the speed commands to the OpenCM board.
     *
     * @return true when data was sent successfully
     */
    bool sendCommandData();

    /**
     * Gets the current angle positions of the drives from the OpenCM board.
     *
     * @return true when angles were received successfully
     */
    bool receiveAngleData();

    /**
     * Callback Function for ohm_inspector command messages.
     *
     * @param commands[in] Received command data
     */
    void commandCallback(const ohm_actors_msgs::Command::ConstPtr& commands);

    /**
     * Callback function for resetting dynamixel drives
     *
     * @param data[in] received data
     */
    void resetCallback(const std_msgs::Bool::ConstPtr& data);

    /**
     * Broadcasts the transform information
     */
    void broadcastTransform();

    /**
     * Wait function.
     * Waits until the timeMS have passed or the user cancels the application (non blocking).
     *
     * @param timeMS[in]: Time to wait in milliseconds
     * @return true when timeMS has passed, false when wait function was aborted
     */
    bool wait(const unsigned int& timeMS);

    const   std::string         _topicName;             /**< Saves the name for the node (needed for communication) */
    const   uint8_t             _maxErrorCnt;           /**< Saves the maximum count of errors which can happen before disconnecting from OpenCM board*/
    const   double              _updateFrequency;       /**< Update frequency of the node in Hz */
    const   float               _speed2DScale;          /**< Scale Factor for the moving speed */
    const   float               _speedAxisScale;        /**< Scale Factor for the speed of the axis for orientation */
    ros::Rate                   _loopRate;              /**< Update timer for the main loop */
    ros::NodeHandle             _nodeHandle;            /**< Instance of the ROS node */
    ros::Subscriber             _cmdSubscriber;         /**< ROS Subscriber for the movement commands */
    ros::Subscriber             _resetSubscriber;       /**< ROS Subscriber for reset dynamixel drives */
    ros::Publisher              _statePublisher;        /**< ROS Publisher: publihs the state of the inspector*/
    OpenCMInterface             _openCMInterface;       /**< Instance to the OpenCM 9.04 Interface class */
    ohm_actors_msgs::Command      _commandData;           /**< Saves the data received from commandCallback() */
    bool                        _newCommandReceived;    /**< Saves true if a new command was received */
    CommunicationState          _comState;              /**< Current state of communication */
    float                       _angles[4];             /**< Array with the current angles of the drives */
    InspectorState              _inspectorState;        /**< Saves the current state of the inspector */

    tf::Transform               _tfInspector[7];        /**< Array with the transform information */
    tf::TransformBroadcaster    _tfBroadcaster;         /**< Instance of tf broadcaster */

};

#endif /* OHM_INSPECTOR_SRC_INSPECTOR_NODE_HPP_ */
