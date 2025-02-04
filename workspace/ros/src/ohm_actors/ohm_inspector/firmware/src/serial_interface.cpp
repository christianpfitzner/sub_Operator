/*
 * serial_interface.cpp
 *
 *  Created on: 04.03.2015
 *      Author: feesma44884
 */
#include "serial_interface.h"

SerialInterface::SerialInterface() :
_dataBufferSize(0), _sendBufferSize(0), _receiveBufferSize(0), _receiveInProgress(false), _dataBufferRead(true), _isConnected(false)  {
    //clear buffer
    for(uint8 i = 0; i < SI_MAX_BUFFER_SIZE; i++) {
        _sendBuffer[i]      =   0;
        _receiveBuffer[i]   =   0;
        _dataBuffer[i] = 0;
    }

}

void SerialInterface::sendDataPacket(const uint8& msg, uint8* data, const uint8 dataSize) {

    //set size
    _sendBufferSize = SI_MIN_MSG_SIZE + dataSize;

    //save message
    _sendBuffer[0]   =   msg;

    //check if there is data to send
    if(dataSize != 0) {
        //append data
        for(uint8 i = 0; i < (dataSize); i++) {
            _sendBuffer[i + 1]   =   data[i];
        }
    }

    //calculate checksum
    uint8 checksum = 0;

    for(uint8 i = 0; i < (_sendBufferSize - 3); i++) {
        checksum += _sendBuffer[i];
    }

    //create message end
    _sendBuffer[_sendBufferSize - 3] = checksum;
    _sendBuffer[_sendBufferSize - 2] = '\r';
    _sendBuffer[_sendBufferSize - 1] = '\n';

    //send data
    SerialUSB.write((unsigned char*)(_sendBuffer), _sendBufferSize);
}

void SerialInterface::callback_dataReceived(byte* data, byte dataSize) {
    //no interrupts until data is processed
    noInterrupts();

    //check if receiving is currently in progress
    if(!_receiveInProgress) {
        //new receiving progress starts -> reset params
        _receiveInProgress = true;
        _receiveBufferSize = 0;
    }

    //check if receiveBufferSize is in range
    if((_receiveBufferSize + dataSize) >= SI_MAX_BUFFER_SIZE) {
        //stop receiving and throw data away
        _receiveInProgress = false;
        _receiveBufferSize = 0;
    }
    else {
        //append data to buffer
        for(uint8 i = 0; i < dataSize; i++) {
            _receiveBuffer[_receiveBufferSize++] = data[i];
        }

        //check if escape sequence was sent
        //minimum of received data is 4
        if(_receiveBufferSize >= SI_MIN_MSG_SIZE) {
            if(_receiveBuffer[_receiveBufferSize - 2] == '\r' && _receiveBuffer[_receiveBufferSize - 1] == '\n') {
                //receiving is finished
                //process received data

                //calculate checksum
                uint8 checksum = 0;

                for(uint8 i = 0; i < (_receiveBufferSize - 3); i++) {
                    checksum += _receiveBuffer[i];
                }

                //check for checksum error
                if(checksum == _receiveBuffer[_receiveBufferSize - 3]) {
                    //checksum is correct -> data is valid

                    //check if connected -> yes send confirmation message
                    if(_isConnected == true) {
                        sendDataPacket((uint8)(_receiveBuffer[0]));
                    }

                    //check if data was processed if not throw data packet away
                    if(_dataBufferRead == true) {
                        //reset databuffer size
                        _dataBufferSize =   0;

                        //data is valid copy to buffer
                        for(uint8 i = 0; i < (_receiveBufferSize - 3); i++) {
                            _dataBuffer[_dataBufferSize++]  =   _receiveBuffer[i];
                        }

                        //data buffer is new -> not readed
                        _dataBufferRead = false;
                    }
                }

                //receive buffer can be overwritten
                _receiveInProgress = false;
            }
        }
    }

    //enable interrupts
    interrupts();
}
