/*
 * opencm_interface.cpp
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */
#include "opencm_interface.hpp"

OpenCMInterface::OpenCMInterface(const uint16_t& timeoutMS, const std::string& deviceName, const RS232::BaudRate& baudRate) :
_vcp(),
_timeoutMS(timeoutMS),
_deviceName(deviceName),
_baudRate(baudRate),
_pingErrorCnt(0),
_messageSent(false)
{

}

bool OpenCMInterface::connect(unsigned int& errorID) {

    //open virtual com port
    if(!_vcp.connect(_deviceName, _baudRate)) {
        return false;
    }

    //try to connect
    if(!sendConnectMessage(_timeoutMS, errorID)) {
        return false;
    }

    //reset vars
    _pingErrorCnt   =   0;
    _messageSent    =   false;

    return true;
}

void OpenCMInterface::disconnect() {
    //disconnect
    _vcp.release();
}

bool OpenCMInterface::keepConnection() {
    //check if a message was sent to OpenCM board
    //if not -> send ping
    if(_messageSent == false) {
        //send ping
        if(!sendPingMessage()) {
            return false;
        }
    }
    //reset var
    _messageSent = false;

    return true;
}

bool OpenCMInterface::sendPingMessage() {
    //send ping message
    if(!sendDataPacket(SI_MSG_PING)) {
        ROS_WARN("OpenCMInterface::sendPing(): Failed!");
        return false;
    }

    //wait for confirm
    if(!waitForConfirm(SI_MSG_PING)) {
        _pingErrorCnt++;
    }
    else {
        _pingErrorCnt = 0;
    }

    //check how much errors occurred
    if(_pingErrorCnt >= 10) {
        _pingErrorCnt = 0;
        return false;
    }

    return true;
}

bool OpenCMInterface::sendStopMessage() {
    //send data
    if(!sendDataPacket(SI_MSG_STOP)) {
        ROS_WARN("OpenCMInterface::sendTimeout(): Failed!");
        return false;
    }

    //wait for confirmation
    if(!waitForConfirm(SI_MSG_STOP)) {
        return false;
    }

    return true;
}

bool OpenCMInterface::sendMoveHomeMessage() {

    //send data
    if(!sendDataPacket(SI_MSG_MOVE_HOME)) {
        ROS_WARN("OpenCMInterface::sendTimeout(): Failed!");
        return false;
    }

    //wait for confirmation
    if(!waitForConfirm(SI_MSG_MOVE_HOME)) {
        return false;
    }

    return true;
}

bool OpenCMInterface::sendMoveParkMessage() {

    //send data
    if(!sendDataPacket(SI_MSG_MOVE_PARK)) {
        ROS_WARN("OpenCMInterface::sendTimeout(): Failed!");
        return false;
    }

    //wait for confirmation
    if(!waitForConfirm(SI_MSG_MOVE_PARK)) {
        return false;
    }

    return true;
}


bool OpenCMInterface::sendMoveCPMessage(const float& speed_X, const float& speed_Y, const float& speed_A, const float& speed_B) {

    //create data packet
    float       fdata[4]    =   {speed_X, speed_Y, speed_A, speed_B};
    uint8_t     data[16];

    //save data
    for(uint8_t i = 0; i < 4; i++) {
        float_pack  conv;

        //save float value
        conv.f  =   fdata[i];

        for(uint8_t j = 0; j < 4; j++) {
            data[i * 4 + j] =   conv.bit[j];
        }
    }
    //send command
    if(!sendDataPacket(SI_MSG_MOVE_CP, data, 16)) {
        return false;
    }

    //wait for confirmation
    if(!waitForConfirm(SI_MSG_MOVE_CP)) {
        return false;
    }

    return true;
}

bool OpenCMInterface::getCurrentAngles(float* angles, InspectorState& state) {

    //create data buffer
    unsigned char dataBuffer[32];
    unsigned int  dataBufferSize;

    //send command
    if(!sendDataPacket(SI_MSG_GET_STATE)) {
        return false;
    }

    //wait for confirmation
    if(!waitForConfirm(SI_MSG_GET_STATE)) {
        return false;
    }

    //wait for data
    if(!_vcp.receiveBuffer(dataBuffer, dataBufferSize, 32, static_cast<unsigned int>(_timeoutMS))) {
        return false;
    }

    //check for right command
    if(dataBuffer[0] != SI_MSG_GET_STATE) {
        return false;
    }

    //check checksum
    if(!checkChecksum(dataBuffer, dataBufferSize)) {
        return false;
    }


    for(uint8_t i = 0; i < 4; i++) {
        float_pack  conv;

        for(uint8_t j = 0; j < 4; j++) {
            conv.bit[j] =   dataBuffer[i * 4 + j + 1];
        }

        //save angle
        angles[i] = conv.f;
    }

    //convert state
    state   =   static_cast<InspectorState>(dataBuffer[17]);

    return true;
}

bool OpenCMInterface::sendDynamixelResetMessage() {
    //send data
    if(!sendDataPacket(SI_MSG_RESET_DYNAMIXEL)) {
        ROS_WARN("OpenCMInterface::sendTimeout(): Failed!");
        return false;
    }

    //wait for confirmation
    if(!waitForConfirm(SI_MSG_RESET_DYNAMIXEL)) {
        return false;
    }
}

bool OpenCMInterface::sendDataPacket(const uint8_t& msg, uint8_t* data, const uint8_t& dataSize) {

    //vars
    uint8_t     sendBuffer[SI_MAX_BUFFER_SIZE];;
    uint8_t     sendBufferSize = (SI_MIN_MSG_SIZE + dataSize);

    //save message
    sendBuffer[0]   =   msg;

    //append data if necessary
    if(dataSize != 0) {
        for(uint8_t i = 0; i < dataSize; i++) {
            sendBuffer[1 + i] = data[i];
        }
    }

    //calculate checksum
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < (sendBufferSize - 3); i++) {
        checksum += sendBuffer[i];
    }

    //create message end
    sendBuffer[sendBufferSize - 3] = checksum;
    sendBuffer[sendBufferSize - 2] = '\r';
    sendBuffer[sendBufferSize - 1] = '\n';

    //send data
    if(!_vcp.transmitBuffer(sendBuffer, sendBufferSize)) {
        return false;
    }

    //a message was sent
    _messageSent = true;

    return true;
}

bool OpenCMInterface::sendConnectMessage(const uint16_t& timeoutMS, unsigned int& errorID) {
    //vars
    unsigned char receiveBuffer[SI_MAX_BUFFER_SIZE];
    unsigned int  receiveBufferSize = 0;
    unsigned int  intTimeoutMS = static_cast<unsigned int>(timeoutMS);

    //split timeout data in two 8 bit
    uint8_t data[2] =   {(timeoutMS>>8), (timeoutMS)};

    //reset error ID
    errorID =   0;

    //send data
    if(!sendDataPacket(SI_MSG_CONNECT, data, 2)) {
        ROS_WARN("OpenCMInterface::sendTimeout(): Failed!");
        return false;
    }

    //wait for confirmation
    //wait for data -> timeout for waiting for data is 150 milliseconds
    if (_vcp.receiveBuffer(receiveBuffer, receiveBufferSize, SI_MAX_BUFFER_SIZE, (intTimeoutMS)) != RS232::SUCCESS) {
        return false;
    }

    //check if size is correct
    if(receiveBufferSize < SI_MIN_MSG_SIZE) {
        ROS_WARN("OpenCMInterface::sendConnectMessage(): Message has wrong data size! (%i)", receiveBufferSize);
        return false;
    }

    //check checksum
    if(!checkChecksum(receiveBuffer, receiveBufferSize)) {
        ROS_WARN("OpenCMInterface::sendConnectMessage(): Checksum error!");
        return false;
    }

    //check received message
    switch(receiveBuffer[0]) {
    case SI_MSG_CONNECT:
    {
        return true;
    }break;
    case SI_MSG_ERROR:
    {
        //save ID of the dynamixel drive
        errorID =   static_cast<int>(receiveBuffer[1]);
        return false;
    }break;
    }

    return false;
}

bool OpenCMInterface::waitForConfirm(const uint8_t& msg) {
    //vars
    unsigned char receiveBuffer[SI_MAX_BUFFER_SIZE];
    unsigned int  receiveBufferSize = 0;
    unsigned int  timeoutMS = static_cast<unsigned int>(_timeoutMS);

    //wait for data -> timeout for waiting for data is 150 milliseconds
    if (_vcp.receiveBuffer(receiveBuffer, receiveBufferSize, SI_MAX_BUFFER_SIZE, (timeoutMS)) != RS232::SUCCESS) {
        return false;
    }

    //check if size is correct
    if(receiveBufferSize != SI_MIN_MSG_SIZE) {
        ROS_WARN("OpenCMInterface::waitForConfirm(): Message has wrong data size! (%i)", receiveBufferSize);
        return false;
    }

    //check checksum
    if(!checkChecksum(receiveBuffer, receiveBufferSize)) {
        ROS_WARN("OpenCMInterface::waitForConfirm(): Checksum error!");
        return false;
    }

    //check if correct message was received
    if(receiveBuffer[0] == msg) {
        return true;
    }

    ROS_WARN("OpenCMInterface::waitForConfirm(): No correct confirm message received (msg:%i)", receiveBuffer[0]);

    return false;
}

bool OpenCMInterface::checkChecksum(unsigned char* data, const unsigned int& dataSize) {
    //calculate checksum
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < (dataSize - 3); i++) {
        checksum += (uint8_t)(data[i]);
    }

    //check checksum in data buffer
    if(data[dataSize - 3] != checksum) {
        return false;
    }

    return true;
}
