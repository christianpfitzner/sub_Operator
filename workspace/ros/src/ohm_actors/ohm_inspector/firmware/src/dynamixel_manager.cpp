/*
 * dynamixel_manager.cpp
 *
 *  Created on: 05.03.2015
 *      Author: feesma44884
 */
#include "dynamixel_manager.h"

namespace DynamixelManager
{

ServoDrive::ServoDrive()
:
_errorCnt(0), _errorCheck(0), _id(0), _model(NONE), _cwLimitDyn(0), _cwLimitRel(0.0f), _ccwLimitDyn(0), _ccwLimitRel(0.0f),
_speedLimitDyn(1), _speedLimitRPM(0.114f), _middAngleDyn(0), _angleDyn(0), _angleRel(0.0f)
{

}

 ServoDrive::ServoDrive(const uint8& id, const Model& model, const int16& middAngle,
                        const int16& cwLimit, const int16& ccwLimit, const float& speedLimitRPM)
:
_errorCnt(0), _errorCheck(0), _id(id), _model(model), _cwLimitDyn(cwLimit), _cwLimitRel(0.0f), _ccwLimitDyn(ccwLimit), _ccwLimitRel(0.0f),
_speedLimitDyn(1), _speedLimitRPM(speedLimitRPM), _middAngleDyn(middAngle), _angleDyn(middAngle), _angleRel(0.0f)
{
    //save parameters
    _angleRel = 0.0f;

    //set cw limit
    _cwLimitDyn =   cwLimit;
    _cwLimitRel =   roundf(DM_DYN_TO_PI(cwLimit - middAngle) * 1000.0f) * 0.001f;

    //set ccw limit
    _ccwLimitDyn =  ccwLimit;
    _ccwLimitRel =  roundf(DM_DYN_TO_PI(ccwLimit - middAngle) * 1000.0f) * 0.001f;

    //check if maximum speed is in range
    if(speedLimitRPM > 117.0f) {
        //set speed limit
        _speedLimitRPM = 117.0f;
        _speedLimitDyn = DM_SPEED_TO_DYN(117.0f);
    }
    else {
        //set speed limit
        _speedLimitRPM = speedLimitRPM;
        _speedLimitDyn = DM_SPEED_TO_DYN(speedLimitRPM);
    }
}

 void ServoDrive::updateAngle(const int16& newAngleDyn) {
     //set new angle
     _angleDyn  =   newAngleDyn;

     //update relative angle
     _angleRel  =   DM_DYN_TO_PI(newAngleDyn - _middAngleDyn);

     //round value
     _angleRel  =   roundf(_angleRel * 1000.0f) * 0.001f;
}

 uint16 ServoDrive::convertRelToAbsAngle(const float& angleRel) {
    //convert
    int16 angleAbs =   static_cast<int16>(DM_PI_TO_DYN(angleRel));

    //add midd position
    angleAbs    +=  _middAngleDyn;

    //check if in range
    if(angleAbs < _cwLimitDyn) {
        angleAbs = _cwLimitDyn;
    }
    else if(angleAbs > _ccwLimitDyn) {
        angleAbs = _ccwLimitDyn;
    }

    return static_cast<uint16>(angleAbs);
}

 uint16 ServoDrive::convertSpeed(const float& speed) {
     //absolute value
     float  absSpeed = roundf(fabsf(speed) * 10.0f) * 0.1f;
     uint16 speedAbs = static_cast<uint16>(DM_SPEED_TO_DYN(absSpeed));

     //check if speed is in range
     if(speedAbs < 1) {
         speedAbs = 1;
     }
     else if(speedAbs > _speedLimitDyn) {
         speedAbs = _speedLimitDyn;
     }

     return speedAbs;
 }

const bool ServoDrive::checkIfAngleIsInRage(const float& angle) {
    //check range
    if(angle > (_ccwLimitRel + 0.002f)) {
        return false;
    }
    else if(angle < (_cwLimitRel - 0.002f)) {
        return false;
    }

    return true;
}

Manager::Manager() :
_timer(HW_TIMER_DYNAMIXEL),
_dynamixelBus(1),
_servoDrives(NULL),
_numServoDrives(0),
_stopActive(false),
_errorCnt(0),
_inRunState(false),
_updateID(0)
{
    //set refresh rate for dynamixels
    _timer.pause();
    _timer.setPeriod(DYNAMIXEL_UPDATE_TIME_US);
    _timer.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
    _timer.setCompare(TIMER_CH1, 1);
    _timer.refresh();
}

void Manager::registerDrives(const ServoDrive* drives, const uint8 numDrives) {
    if(_numServoDrives != 0)    release();

    //allocate memory for drives
    _numServoDrives =   numDrives;
    _servoDrives    =   (ServoDrive*)(malloc(_numServoDrives * sizeof(ServoDrive)));

    //copy data
    for(uint8 i = 0; i < _numServoDrives; i++) {
       _servoDrives[i] =   drives[i];
    }
}

Manager::~Manager() {
    //check if there are still drives allocated
    if(_numServoDrives != 0)    release();
}

void Manager::release() {
    //check if there are drives allocated
    if(_numServoDrives == 0)    return;

    //release allocated memory
    SAFE_DELETE(_servoDrives);

    //no memory for drives allocated
    _numServoDrives =   0;
}

bool Manager::initRunState() {
    //check if drives are allocated
    if(_numServoDrives == 0)    return false;

    //reset error cnt
    _errorCnt = 0;

    //Init dynamixel drives -> BaudRate 57600
    _dynamixelBus.begin(1);

    //loop through all dynamixel drives
    for(uint8 i = 0; i < _numServoDrives; i++) {
        //Pointer to the servo drives
        ServoDrive* drive   =   &_servoDrives[i];

        //get model number of dynamixel drive
        word model  =   _dynamixelBus.getModelNumber(drive->getID());

        //check if model is correct
        if(model != static_cast<word>(drive->getModel())) {
            //wrong model number
            return false;
        }

        //set drive to joint mode
        _dynamixelBus.jointMode(drive->getID()); delay(2);

        //write limits
        _dynamixelBus.writeWord(drive->getID(), 6, drive->getCWLimitDyn()); delay(2);
        _dynamixelBus.writeWord(drive->getID(), 8, drive->getCCWLimitDyn()); delay(2);

        //write torque to max
        _dynamixelBus.writeWord(drive->getID(), 14, 1023); delay(2);

        //set speed to minimum 1.14 rpm
        _dynamixelBus.writeWord(drive->getID(), 32, 10); delay(2);

        //lock EEPROM
        _dynamixelBus.writeByte(drive->getID(), 47, 1);
    }

    //run state is active
    _inRunState =   true;
    _stopActive =   false;

    //vars
    bool validPositions = true;

    //allocate memory
    float angles[_numServoDrives];
    float speeds[_numServoDrives];

    //set data
    for(uint8 i = 0; i < _numServoDrives; i++) {
        speeds[i]   =   0.5f;

        switch(i) {
        case 0: angles[i]   =   -1.57f; break;
        case 1: angles[i]   =   -1.57f; break;
        case 2: angles[i]   =   -1.55f; break;
        case 3: angles[i]   =    1.57f; break;
        }
    }

    //wait
    delay(100);

    //send move message
    moveServoDrivesSimultanous(_numServoDrives, angles, speeds); _timer.pause();
    delay(120);
    moveServoDrivesSimultanous(_numServoDrives, angles, speeds); _timer.pause();
    delay(120);
    moveServoDrivesSimultanous(_numServoDrives, angles, speeds); _timer.pause();

    //set run state to false
    _inRunState =   false;

    //try to get valid position
    _updateID = 0;
    for(uint8 i = 0; i < _numServoDrives; i++) {
        if(!updateAngles()) {
            validPositions = false;
            break;
        }
        delay(10);
        toggleLED();
    }

    //check if in run state
    if(!validPositions) {
        //run state is false
        _inRunState = false;
    }
    else {
        //run state is true
        _inRunState = true;

        //restart timer
        _timer.resume();

        //wait
        for(uint8 i = 0; i < 200; i++) {
            delay(1);
        }

        //stop movement
        stopMovement();
    }

    return _inRunState;
}

void Manager::exitRunState() {
    //stop timer
    _timer.pause();

    //run state is deactivated
    _inRunState =   false;

    //stop all movement
    stopMovement();
}

bool Manager::moveToTargetAngle(const uint8& id, const float& targetAngleRel){
    //check if drives are in run state
    if(_inRunState == false)    return false;
    bool result = true;

    //pointer to drive
    ServoDrive* drive   =   &_servoDrives[id];

    //get target angle
    uint16   targetAngleDyn =   drive->convertRelToAbsAngle(targetAngleRel);

    //stop timer
    _timer.pause();

    //approach new position
    _dynamixelBus.writeWord(drive->getID(), 30, targetAngleDyn);

    //check if result is ok
    if(_dynamixelBus.getResult() != 2) {
        result = false;
    }

    //start timer
    _timer.resume();

    //stop is no longer active
    _stopActive =   false;

    return result;
}

bool Manager::moveToTargetAngle(const uint8& id, const float& targetAngleRel, const float& targetSpeed) {
    //check if drives are in run state
    if(_inRunState == false)    return false;
    bool result = true;

    //pointer to drive
    ServoDrive* drive   =   &_servoDrives[id];

    //stop timer
    _timer.pause();

    //approach new position
    _dynamixelBus.setPosition(drive->getID(), drive->convertRelToAbsAngle(targetAngleRel), drive->convertSpeed(targetSpeed));

    //check if result is ok
    if(_dynamixelBus.getResult() != 2) {
        result =  false;
    }

    //start timer
    _timer.resume();

    //stop is no longer active
    _stopActive =   false;

    return result;
}

bool Manager::moveServoDrivesSimultanous(const uint8& maxID, const float targetAnglesRel[],
                                         const float targetSpeedsRPM[]) {
    //check if drives are in run state
    if(_inRunState == false)    return false;
    bool result = true;

    //stop timer
    _timer.pause();

    //no interrupts until data is sent
    noInterrupts();

    //setup data packet for sync write
    _dynamixelBus.setTxPacketId(BROADCAST_ID);
    _dynamixelBus.setTxPacketInstruction(INST_SYNC_WRITE);

    //Adress for GoalPosition is 30, 31 and MovingSpeed 32, 33
    //Start Adress = 30 and four Bytes are send
    _dynamixelBus.setTxPacketParameter(0, 30);
    _dynamixelBus.setTxPacketParameter(1, 4);

    //write data for every servo drive
    for(uint8 i = 0; i < maxID; i++) {
        uint16  angleDyn    =   _servoDrives[i].convertRelToAbsAngle(targetAnglesRel[i]);
        uint16  speed       =   _servoDrives[i].convertSpeed(targetSpeedsRPM[i]);

        _dynamixelBus.setTxPacketParameter(2 + 5 * i, _servoDrives[i].getID());
        _dynamixelBus.setTxPacketParameter(2 + 5 * i + 1, DXL_LOBYTE(angleDyn));
        _dynamixelBus.setTxPacketParameter(2 + 5 * i + 2, DXL_HIBYTE(angleDyn));
        _dynamixelBus.setTxPacketParameter(2 + 5 * i + 3, DXL_LOBYTE(speed));
        _dynamixelBus.setTxPacketParameter(2 + 5 * i + 4, DXL_HIBYTE(speed));
    }

    //setup data length
    _dynamixelBus.setTxPacketLength(2 + 5 * maxID);

    //enable interrupts
    interrupts();

    //send data
    _dynamixelBus.txrxPacket();

    if(_dynamixelBus.getResult() != 2) {
          result = false;
    }

    //start timer
    _timer.resume();

    //stop is no longer active
    _stopActive =   false;

    return result;
}

void Manager::stopMovement() {
    if(_stopActive == true) return;

    //allocate memory
    float targetAngles[_numServoDrives];
    float targetSpeeds[_numServoDrives];

    //stop timer
    _timer.pause();

    //write positions
    for(uint8 i = 0; i < _numServoDrives; i++) {
        targetAngles[i] =   _servoDrives[i].getAngleRel();
        targetSpeeds[i] =   0.5f;
    }

    //move drives simultanous
    moveServoDrivesSimultanous(_numServoDrives, targetAngles, targetSpeeds);

    //wait
    delay(20);

    //check if is in run state
    if(_inRunState == true) {
        //start timer
        _timer.resume();

        //stop is active
        _stopActive =   true;
    }
}

bool Manager::updateAngles() {
    bool result = true;

    //update drive
    ServoDrive* drive = &_servoDrives[_updateID];

    //get position
    int16 angle = static_cast<int16>(_dynamixelBus.getPosition(drive->getID()));

    //check if position is valid
    if(_dynamixelBus.getResult() != 2) {
        result = false;
    }
    else {
        //check if position has changed for more than 2 digits
        if(abs(drive->getAngleDyn() - angle) > 2) {
            //set new position
            drive->updateAngle(angle);
        }
    }

    //check if dynamixel manager is in run state
    if(_inRunState) {
        //increase cnt
        drive->_errorCheck ++;

        //every second time check error
        if(drive->_errorCheck > 6) {
            //check model -> wrong value returned if error
            delay(2);
            _dynamixelBus.readByte(drive->getID(), 0x02);
            byte   dynResult =   _dynamixelBus.getResult();

            //check if correct value was received
            if(dynResult == 2) {
                drive->_errorCnt    =   0;
            }
            else {
                drive->_errorCnt++;

                //check if to many errors occured
                if(drive->_errorCnt >= DYNAMIXEL_MAX_ERROR_CNT) {
                    exitRunState();
                }
            }

            drive->_errorCheck  = 0;
        }
    }
    else {
        drive->_errorCnt    =   0;
    }

    //update next drive
    _updateID++;

    if(_updateID >= _numServoDrives) {
        _updateID = 0;
    }

    return result;
}

void Manager::setPunch(const uint8& id, const uint16& punch) {

    //set punch for the selected drive
    _dynamixelBus.writeWord(_servoDrives[id].getID(), 48, punch);

}

void Manager::callback_timer() {

    //check if in run state
    if(_inRunState == true) {
        bool error = false;

        //read firmware
        _dynamixelBus.readByte(_servoDrives[_updateID].getID(), 0x02);

        //check if error occured
        if(_dynamixelBus.getResult() != 2) {
            error = true;
        }

        //update all angles
        if(!updateAngles()) {
            error = true;
        }

        //check if error occured
        if(error)     {_errorCnt++;}
        else          {_errorCnt = 0;}

        //check how much errors occured
        if(_errorCnt > DM_MAX_NUM_ERRORS) {
            //disconnect, because of to much errors
            _timer.pause();
            _inRunState = false;
        }
    }
}

} /*namespace DynamixelManager*/
