#include "firmware.h"

//DO NOT EDIT!!!
 __attribute__(( constructor )) void premain() {
    init();
}
 //!DO NOT EDIT!!!
 Firmware g_firmware;

 void serial_callbackfunc(byte* data, byte dataSize) {
     g_firmware._serialInterface.callback_dataReceived(data, dataSize);
 }

 void timer1_callbackfunc() {
     g_firmware._dynamixelManager.callback_timer();
 }

Firmware::Firmware() :
_serialInterface(),
_dynamixelManager(),
_inspectorControl(),
_connectionState(STATE_WAIT_FOR_DYNAMIXELS),
_inspectorState(STATE_PARKED),
_inspectorCMD(CMD_NONE),
_timerStamp(0),
_timerMS(0),
_timeoutStamp(0),
_timeoutMS(0)
{
    //reset vars
    for(uint8 i = 0; i < 4; i++) {
        _cp_Speeds[i] = 0.0f;
    }

    //attach interrupt
    SerialUSB.attachInterrupt(serial_callbackfunc);

    //attach timer interrupt
    _dynamixelManager.getTimer()->attachInterrupt(TIMER_CH1, timer1_callbackfunc);
}

void Firmware::init() {
    //wait
    delay(100);

    //set LED pin as output
    pinMode(BOARD_LED_PIN, OUTPUT);

    pinMode(20, OUTPUT);
    digitalWrite(20, HIGH);

    //toggle LED
    digitalWrite(BOARD_LED_PIN, HIGH);
    for(uint8 i = 0; i < 10; i++) {
        toggleLED(); delay(50);
    }

    //set up drives
    setupServoDrives();

    //setup inspector control
    _inspectorControl   =   Inspector::Control(&_dynamixelManager,
                                               Inspector::Dimension(INSPECTOR_L_0_MM, INSPECTOR_L_1_MM, INSPECTOR_L_2_MM, INSPECTOR_L_3_MM));

    //set collision factors
    _inspectorControl.setCollisionFactors(INSPECTOR_COLLISION_DISTANCE_X, INSPECTOR_COLLISION_DISTANCE_Y);
}

void Firmware::messageLoop() {
    //program loop
    while(true) {
    //init watchdog timer
    //iwdg_init(IWDG_PRE_64, 4000);

        //calculate timer
        _timerMS    =   static_cast<uint16>(millis() - _timerStamp);

        //check states
        switch(_connectionState) {
        case STATE_WAIT_FOR_DYNAMIXELS:
        {
            //State none -> try to connect to dynamixels to get in state wait for connection
            if(connectDynamixels()) {
                _connectionState   =   STATE_WAIT_FOR_HOST;
            }
            else {
                refuseConnectionRequest();
            }
        }break;
        case STATE_WAIT_FOR_HOST:
        {
             
            //check if dynamixels are connected
            if(!_dynamixelManager.getRunState()) {
                _connectionState = STATE_WAIT_FOR_DYNAMIXELS;
            }
            else
            {
                //State wait for host -> wait for host to establish connection to get in idle state
                if(waitForHost()) {
                    //dynamixels and host connected
                    _connectionState = STATE_CONNECTED;

                    //check in which state the inspector is
                    //parked or moving state
                    checkInspectorState();

                    //switch on LED
                    digitalWrite(BOARD_LED_PIN, LOW);
                }
            }
        }break;
        case STATE_CONNECTED:
        {
            if(!checkConnectionState()) {
                //switch off LED
                digitalWrite(BOARD_LED_PIN, HIGH);
            }
            else {
                processReceivedData();

                run();
            }
        }break;
        }

        //reset iwdg
        //iwdg_feed();
    }

}

void Firmware::release() {
    //release dynamixel manager
    _dynamixelManager.release();
}

void Firmware::run() {
    //check if inspector is parked
    if(_inspectorState == STATE_PARKED) {
        //show parking state with  blinking LED
        if(_timerMS > 200) {
            toggleLED();
            resetTimer();
        }
    }
    else {
      digitalWrite(BOARD_LED_PIN, LOW);
    }

    //process received command
    switch(_inspectorCMD) {
    case CMD_NONE:
    {
        //check if inspector has to stop
        if(_inspectorState == STATE_MOVE_PARK && !checkIfInspectorIsParked()) {
            _dynamixelManager.stopMovement();
            _inspectorControl.reset_Orientation();
        }
        else if(_inspectorState == STATE_MOVE_HOME && !checkIfInspectorIsHome()) {
            _dynamixelManager.stopMovement();
            _inspectorControl.reset_Orientation();
        }
        else{
            for(uint8 i = 0; i < 4; i++) {
                _cp_Speeds[i]   =   0.0f;
            }
        }

        if(_inspectorState != STATE_PARKED) {
            _inspectorState =   STATE_RELEASED;
        }
    }break;
    case CMD_PARK:
    {
        //check if inspector is not in park state
        if(_inspectorState != STATE_PARKED) {
            //check if inspector is in home position
            bool isHome =   checkIfInspectorIsHome();

            //if inspector is not in home position move to it first
            if(!isHome && _inspectorState != STATE_MOVE_HOME && _inspectorState != STATE_MOVE_PARK) {
                _inspectorControl.move_ToHomePosition();
                _inspectorState = STATE_MOVE_HOME;
            }
            //check if inspector is home
            else if(isHome && _inspectorState != STATE_MOVE_PARK) {
                //move to park position
                _inspectorControl.move_ToParkPosition();
                _inspectorState = STATE_MOVE_PARK;
            }
            else if(checkIfInspectorIsParked()) {
                _inspectorState = STATE_PARKED;
            }

        }
    }break;
    case CMD_HOME:
    {
        //check if inspector is in home position
        if(!checkIfInspectorIsHome()) {
            //check if move home command was sent
            if(_inspectorState != STATE_MOVE_HOME) {
                _inspectorControl.move_ToHomePosition();
                _inspectorState = STATE_MOVE_HOME;
            }
            else {
                //check if home position is reached
                if(checkIfInspectorIsHome()) {
                    _inspectorState =   STATE_RELEASED;
                }
            }
        }
        else {
            _inspectorState =   STATE_RELEASED;
        }
    }break;
    case CMD_MOVE:
    {
        //check if movement is release
        if(_inspectorState == STATE_RELEASED) {
            //move inspector
            _inspectorControl.run(_cp_Speeds[0], _cp_Speeds[1], _cp_Speeds[2], _cp_Speeds[3]);
        }

    }break;
    }
}

void Firmware::setupServoDrives() {
    DynamixelManager::ServoDrive    drives[4];
    //drives[0]   =   DynamixelManager::ServoDrive(5, DynamixelManager::MX28, 1024, 0, 2048, 5.0f);

    //Drive for positioning
    drives[0]   =   DynamixelManager::ServoDrive(2, DynamixelManager::MX106, 2048, 1035, 2790, DYNAMIXEL_MAX_RPM);
    drives[1]   =   DynamixelManager::ServoDrive(3, DynamixelManager::MX106, 2048, 1036, 3300, DYNAMIXEL_MAX_RPM);

    //Drives for orientation
    drives[2]   =   DynamixelManager::ServoDrive(4, DynamixelManager::MX28, 1024, 0, 2048, INSPECTOR_MAX_ORIENT_SPEED);
    drives[3]   =   DynamixelManager::ServoDrive(5, DynamixelManager::MX28, 2048, 900, 3100, INSPECTOR_MAX_ORIENT_SPEED);

    _dynamixelManager.registerDrives(drives, 4);

    float angles[4] =   {0.0f, 0.0f, 0.0f, 0.0f};
    float speeds[4] =   {2.0f, 2.0f, 2.0f, 2.0f};
    _dynamixelManager.moveServoDrivesSimultanous(4, angles, speeds);
}

bool Firmware::connectDynamixels() {

    //try to establish connection to dynamixel drives all 1000 ms
    if(_timerMS > 1000) {
        //try to init run state
        if(_dynamixelManager.initRunState()) {
            return true;
        }

        //blink LED
        toggleLED();

        resetTimer();
    }

    //failed to establish connection
    return false;

    return true;
}

bool Firmware::waitForHost() {
    //check if host has send connection request
    processReceivedData();

    //check if connection was established
    if(_serialInterface.isConnected()) {
        return true;
    }

    //blink LED
    if(_timerMS > 500) {
        toggleLED();
        resetTimer();
    }

    return false;
}

bool Firmware::checkConnectionState() {

    bool disconnect =   false;

    //check if dynamixels are connected
    if(!_dynamixelManager.getRunState()) {
        //reset state to none
        _connectionState   =   STATE_WAIT_FOR_DYNAMIXELS;

        disconnect = true;
    }

    //check if host is connected
    if(!_serialInterface.isConnected()) {
        //fall back to wait for host
        _connectionState   =   STATE_WAIT_FOR_HOST;

        disconnect = true;
    }

    //check if timeout occured
    if(checkTimeout()) {
        //fall back to wait for host, because of timeout
        _connectionState   =   STATE_WAIT_FOR_HOST;

        disconnect = true;
    }

    //check if disconnect
    if(disconnect) {
        //stop movement
        _dynamixelManager.stopMovement();

        //diconnect
        _serialInterface.disconnected();

        //reset vars
        for(uint8 i = 0; i < 4; i++) {
            _cp_Speeds[i] = 0.0f;
        }

        return false;
    }

    return true;
}

bool Firmware::checkIfInspectorIsParked() {
    //get angles
    float   angles[4]       =   {0.0f, 0.0f, 0.0f, 0.0f};
    float   parkAngles[4]   =   INSPECTOR_PARK_POSITION;
    bool    parkPos = true;

    //get angles
    for(uint8 i = 0; i < 4; i++) {
        angles[i]   =   fabsf(_dynamixelManager.getServoDrive(i)->getAngleRel() - parkAngles[i]);

        //check if angle is near park pos
        if(angles[i] > 0.3f) {
            parkPos = false;
        }
    }

    return parkPos;
}

bool Firmware::checkIfInspectorIsHome() {
    //get angles
    float   angles[4]       =   {0.0f, 0.0f, 0.0f, 0.0f};
    float   homeAngles[4]   =   INSPECTOR_HOME_POSITION;
    bool    homePos = true;

    //get angles
    for(uint8 i = 0; i < 4; i++) {
        angles[i]   =   fabsf(_dynamixelManager.getServoDrive(i)->getAngleRel() - homeAngles[i]);

        //check if angle is near park pos
        if(angles[i] > 0.2f) {
            homePos = false;
        }
    }

    return homePos;
}

void Firmware::checkInspectorState() {

    //check if inspector is in park position
    if(checkIfInspectorIsParked()) {
        _inspectorState   =   STATE_PARKED;
    }
    else {
        _inspectorState   =   STATE_RELEASED;
    }

}

void Firmware::processReceivedData() {
    uint8   buffer[SI_MAX_BUFFER_SIZE];
    uint8   bufferSize = 0;

    //check if there is new data
    if(!_serialInterface.newDataAvailable()) return;

    //copy data
    bufferSize = (uint8)_serialInterface._dataBufferSize;
    for(uint8 i = 0; i < _serialInterface._dataBufferSize; i++) {
        buffer[i] = (uint8)_serialInterface._dataBuffer[i];
    }

    //data was read
    _serialInterface.dataBufferRead();

    //check command
    switch(buffer[0]) {
    case SI_MSG_CONNECT:
    {
        //check if data size is correct
        if(bufferSize == (1 + 2)) {
            //timeout was send extract data and set timeout
            _timeoutMS   = ((uint16)buffer[1] << 8) | (uint16)(buffer[2]);

            //connection established
            _serialInterface.connected();

            //set led on
            digitalWrite(BOARD_LED_PIN, LOW);

            //confirm with msg
            _serialInterface.sendDataPacket(SI_MSG_CONNECT);
        }
    }break;
    case SI_MSG_PING:
    {
    }break;
    case SI_MSG_STOP:
    {
        _inspectorCMD   = CMD_NONE;
    }break;

    case SI_MSG_MOVE_HOME:
    {
        _inspectorCMD   =   CMD_HOME;
    }break;

    case SI_MSG_MOVE_PARK:
    {
        _inspectorCMD   =   CMD_PARK;
    }break;

    case SI_MSG_MOVE_CP:
    {
      if(_serialInterface.isConnected() && bufferSize == (1 + 16)) {
          //move
          _inspectorCMD =   CMD_MOVE;

            //extract data
            for(uint8 i = 0; i < 4; i++) {
                //converter
                float_pack  conv;

                for(uint8 j = 0; j < 4; j++) {
                    conv.bit[j] =   (uint8)(buffer[1 + i * 4 + j]);
                }

                //save speed
                _cp_Speeds[i]   =   conv.f;

                //range speed
                if(i == 0 || i == 1) {
                    if(_cp_Speeds[i] > INSPECTOR_MAX_SPEED)          {_cp_Speeds[i] = INSPECTOR_MAX_SPEED;}
                    else if(_cp_Speeds[i] < -INSPECTOR_MAX_SPEED)    {_cp_Speeds[i] = -INSPECTOR_MAX_SPEED;}
                }
                else {
                    if(_cp_Speeds[i] > INSPECTOR_MAX_ORIENT_SPEED)          {_cp_Speeds[i] = INSPECTOR_MAX_ORIENT_SPEED;}
                    else if(_cp_Speeds[i] < -INSPECTOR_MAX_ORIENT_SPEED)    {_cp_Speeds[i] = -INSPECTOR_MAX_ORIENT_SPEED;}
                }

            }

        }
    }break;

    case SI_MSG_GET_STATE:
    {
        //pack data
        uint8   sendBuffer[17];
        uint8   sendBufferSize = 17;

        for(uint8 i = 0; i < 4; i++) {
            float_pack conv;
            conv.f  =   _dynamixelManager.getServoDrive(i)->getAngleRel();

            //save data to buffer
            for(uint8 j = 0; j < 4; j++) {
                sendBuffer[i * 4 + j]   =   conv.bit[j];
            }
        }

        //last byte contains the current state
        sendBuffer[16] =   static_cast<uint8>(_inspectorState);

        //send data
        _serialInterface.sendDataPacket(SI_MSG_GET_STATE, sendBuffer, sendBufferSize);

    }break;

    case SI_MSG_RESET_DYNAMIXEL:
    {
        //reset dynamixels
        digitalWrite(20, LOW);
        _connectionState    =   STATE_WAIT_FOR_DYNAMIXELS;
        _inspectorCMD       =   CMD_NONE;
	_serialInterface.disconnected();
        _dynamixelManager.exitRunState();
        for(uint16 i = 0; i < 1000; i++) {
            delay(1);
        }
        digitalWrite(20, HIGH);
    }break;
    }


    //reset timeout again
    if(_serialInterface.isConnected() == true)    resetTimeout();
}

void Firmware::refuseConnectionRequest() {
    uint8   buffer[12];
    uint8   bufferSize = 0;

    //check if there is new data available
    if(!_serialInterface.newDataAvailable())    return;

    //copy data
    bufferSize = (uint8)_serialInterface._dataBufferSize;
    for(uint8 i = 0; i < _serialInterface._dataBufferSize; i++) {
        buffer[i] = (uint8)_serialInterface._dataBuffer[i];
    }

    //data was read
    _serialInterface.dataBufferRead();

    //check command
    switch(buffer[0]) {
    case SI_MSG_CONNECT:
    {
        //check if data size is correct
        if(bufferSize == (1 + 2)) {
            //refuse connection request and send back error code
            uint8   data[1] =   {0};

            //check which dynamixel drive has an error
            for(uint8 i = 0; i < _dynamixelManager.getNumServoDrives(); i++) {
                if(_dynamixelManager.getServoDrive(i)->_errorCnt >= DYNAMIXEL_MAX_ERROR_CNT) {
                    data[0] =   _dynamixelManager.getServoDrive(i)->getID();
                    i = _dynamixelManager.getNumServoDrives();
                }
            }

            _serialInterface.sendDataPacket(SI_MSG_ERROR, data, 1);
        }
    }break;
    }

}

void Firmware::resetTimeout() {
    _timeoutStamp = millis();
}

bool Firmware::checkTimeout() {

    //check if timeout occured
    if((uint16)(millis() - _timeoutStamp) > _timeoutMS) {
        return true;
    }

    return false;
}

int main(void) {

    //initialize opencm firmware
    g_firmware.init();

    //run firmware
    g_firmware.messageLoop();

    //release memory
    g_firmware.release();

    return 0;
}

