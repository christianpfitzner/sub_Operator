/*
 * inspector.cpp
 *
 *  Created on: 06.03.2015
 *      Author: feesma44884
 */
#include "inspector.h"

namespace Inspector
{
Dimension::Dimension() :
_length_0(0.0f),
_length_1(0.0f),
_length_2(0.0f),
_length_3(0.0f),
_length_3_SQ(0.0f),
_length_12(0.0f),
_length_12_SQ(0.0f),
_angle_12(0.0f),
_maxDistance(0.0f)
{

}

Dimension::Dimension(const float& length_0, const float& length_1, const float& length_2, const float& length_3) :
_length_0(length_0),
_length_1(length_1),
_length_2(length_2),
_length_3(length_3),
_length_3_SQ(0.0f),
_length_12(0.0f),
_length_12_SQ(0.0f),
_angle_12(0.0f),
_maxDistance(0.0f)
{
    //calculate square of length 3
    _length_3_SQ    =   powf(_length_3, 2);

    //calculate the square of length 12
    _length_12_SQ   =   powf(_length_1, 2) + powf(_length_2, 2);

    //calculate length 12
    _length_12      =   sqrtf(_length_12_SQ);

    //calculate angle 12
    _angle_12   =   atanf(_length_2 / _length_1);

    //calculate max distance the inspector can reach, referred to the first pivot axis
    _maxDistance    =   _length_12 + _length_3;
}

Control::Control() :
_dimensions(),
_dynamixelManager(NULL),
_cp_TimeStamp(0),
_cp_TimeStep(0.0f),
_cp_NewTimeStep(0.0f),
_cp_Moving(false),
_cp_Target_X(0.0f),
_cp_Target_Y(0.0f),
_cp_Speed_X(0.0f),
_cp_Speed_Y(0.0f),
_orient_Block_Time(0.0f),
_orient_TimeStamp(0),
_orient_Theta1(0.0f),
_orient_Theta2(0.0f),
_orient_Speed1(0.0f),
_orient_Speed2(0.0f),
_orient_Move(false),
_tcp_PosX(0.0f),
_tcp_PosY(0.0f),
_planeX(InspectorMath::Vector3(1.0f, 0.0f, 0.0f), InspectorMath::Vector3()),
_planeY(InspectorMath::Vector3(0.0f, 1.0f, 0.0f), InspectorMath::Vector3())
{

}

Control::Control(DynamixelManager::Manager* dynMan, const Dimension& dimensions) :
_dimensions(dimensions),
_dynamixelManager(dynMan),
_cp_TimeStamp(0),
_cp_TimeStep(0.0f),
_cp_NewTimeStep(0.0f),
_cp_Moving(false),
_cp_Target_X(0.0f),
_cp_Target_Y(0.0f),
_cp_Speed_X(0.0f),
_cp_Speed_Y(0.0f),
_orient_Block_Time(0.0f),
_orient_TimeStamp(0),
_orient_Theta1(0.0f),
_orient_Theta2(0.0f),
_orient_Speed1(0.0f),
_orient_Speed2(0.0f),
_orient_Move(false),
_tcp_PosX(0.0f),
_tcp_PosY(0.0f),
_planeX(InspectorMath::Vector3(1.0f, 0.0f, 0.0f), InspectorMath::Vector3()),
_planeY(InspectorMath::Vector3(0.0f, 1.0f, 0.0f), InspectorMath::Vector3())
{

}

void Control::setCollisionFactors(const float& rangeX, const float rangeY) {

    _planeX =   InspectorMath::Plane(InspectorMath::Vector3(1.0f, 0.0f, 0.0f), InspectorMath::Vector3(rangeX, 0.0f, 0.0f));
    _planeY =   InspectorMath::Plane(InspectorMath::Vector3(0.0f, 1.0f, 0.0f), InspectorMath::Vector3(0.0f, rangeY, 0.0f));

}

bool Control::run(const float& speed_X, const float& speed_Y, const float& speed_A, const float& speed_B) {

    //save speed
    _cp_Speed_X = speed_X;
    _cp_Speed_Y = speed_Y;

    _orient_Speed1 = speed_A;
    _orient_Speed2 = speed_B;

    checkCollision();
    move_ContinuousPath();
    move_Orientation();

    return true;
}

void Control::move_ToHomePosition() {
    //move drives

    float   angles[4]   =   INSPECTOR_HOME_POSITION;
    move_SynchronousToTargetAngles(0.8f, angles, 4);

    //reset vars
    _orient_Theta1      =   angles[2];
    _orient_Theta2      =   0.0f;

    //block orientation
    block_Orientation(1.0f);

}

void Control::move_ToParkPosition() {
    //move drives
    float   angles[4]   =   INSPECTOR_PARK_POSITION;
    float   speeds[4]   =   {10.0f, 10.0f, 50.0f, 50.0f};
    _dynamixelManager->moveServoDrivesSimultanous(4, angles, speeds);

    //reset vars
    _orient_Theta1      =   angles[2];
    _orient_Theta2      =   1.7f;

    //block orientation
    block_Orientation(1.0f);
}

void Control::reset_Orientation() {
    //set vars
    _orient_Theta1  =   _dynamixelManager->getServoDrive(2)->getAngleRel();
    _orient_Theta2  =   _dynamixelManager->getServoDrive(3)->getAngleRel();
}

void Control::calculate_TCPPosition(const float& theta_1, const float& theta_2, float& position_X, float& position_Y) {
    //calculate x - position
    position_X   =  _dimensions._length_1 * sinf(theta_1) + _dimensions._length_2 * cosf(theta_1) +
                    _dimensions._length_3 * cosf(theta_2 - theta_1);

    //calculate y - position
    position_Y  =   _dimensions._length_0 + _dimensions._length_1 * cosf(theta_1) +
                    _dimensions._length_2 * sinf(-theta_1) + _dimensions._length_3 * sinf(theta_2 - theta_1);
}

bool Control::calculate_TCPAngles(const float& position_X, const float& position_Y, float& theta_1, float& theta_2) {
    //calculate constant params+
    const float X           =   position_X;
    const float Y           =   position_Y - _dimensions._length_0;
    const float lenght_P_SQ =   X * X + Y * Y;
    const float length_P    =   sqrtf(lenght_P_SQ);
    const float gamma       =   atan2f(Y, X);

    //check if the tool center point position is reachable
    if(length_P >= (0.989f * _dimensions._maxDistance) || length_P <= (0.1f * _dimensions._maxDistance)) {
        //position is to far away or to near -> abort
        return false;
    }

    //calculate alpha angle
    float   alphaFac    =   lenght_P_SQ + _dimensions._length_12_SQ - _dimensions._length_3_SQ;
    float   alphaDiv    =   2.0f * _dimensions._length_12 * length_P;
    float   alpha       =   gamma + acosf(alphaFac / alphaDiv);

   //calculate beta angle
   float   betaFac     =   _dimensions._length_12_SQ + _dimensions._length_3_SQ - lenght_P_SQ;
   float   betaDiv     =   2.0f * _dimensions._length_12 * _dimensions._length_3;
   float   beta        =   acosf(betaFac / betaDiv);

   //calculate angles
   theta_1  =   -(alpha + _dimensions._angle_12 - (PI * 0.5f));
   theta_2  =   (beta - _dimensions._angle_12 - (PI * 0.5f));

    return true;
}

bool Control::move_SynchronousToTargetAngles(const float& time, const float* targetAngles, const uint8& numAngles) {

    //vars
    float  targetSpeeds[numAngles];
    float  deltaAngle      =   0.0f;


    //go through all drives
    for(uint8 i = 0; i < numAngles; i++) {
        //get pointer to drive
        DynamixelManager::ServoDrive* drive =   _dynamixelManager->getServoDrive(i);

        //calculate angle step
        deltaAngle  =   roundf(targetAngles[i] * 1000.0f) * 0.001f - drive->getAngleRel();

        //calculate speed
        targetSpeeds[i]    =   fabsf((deltaAngle / time) / (2.0f * PI)) * 60.0f;
    }

    //search for fastest drive
    uint8 fastestDriveID = 0;
    float maxSpeed = 0.0f;
    for(uint8 i = 0; i < numAngles; i++) {
        if(targetSpeeds[i] > maxSpeed) {
            maxSpeed = targetSpeeds[i];
            fastestDriveID = i;
        }
    }

    //check if fastest drive reaches speed limits
    if(targetSpeeds[fastestDriveID] > _dynamixelManager->getServoDrive(fastestDriveID)->getMaxSpeedRPM()) {
        float   f = _dynamixelManager->getServoDrive(fastestDriveID)->getMaxSpeedRPM() / targetSpeeds[fastestDriveID];

        for(uint8 i = 0; i < numAngles; i++) {
            targetSpeeds[i] *= f;
        }
    }

    //move drives
    _dynamixelManager->moveServoDrivesSimultanous(numAngles, targetAngles, targetSpeeds);

    return true;
}

bool Control::move_ContinuousPath() {
    //vars
    bool    newTarget   =   false;

    //calculate passed time in seconds
    float   dTime   =   (float)(millis() - _cp_TimeStamp) * 0.001f;

    //check if delta time has passed
    if(dTime >= INSPECTOR_CONTROL_CALC_TIME_STEP) {

        //calculate speed vector
        float   speedVec    =   sqrtf(_cp_Speed_X * _cp_Speed_X + _cp_Speed_Y * _cp_Speed_Y);

        //check if there is a movement and if the time step has passed
        if(speedVec >= 2.5f) {
            float   newTarget_X = _cp_Target_X;
            float   newTarget_Y = _cp_Target_Y;
            float   time = 0.0f;

            //check if inspector starts moving
            if(_cp_Moving == false) {

                //calculate current position
                calculate_TCPPosition(_dynamixelManager->getServoDrive(0)->getAngleRel(),
                                      _dynamixelManager->getServoDrive(1)->getAngleRel(),
                                      _cp_Target_X, _cp_Target_Y);

                //correction for y position
                _cp_Target_Y += 10.0f;

                //calculate target
                newTarget_X =   _cp_Target_X + _cp_Speed_X * INSPECTOR_CONTROL_MOVE_TIME_STEP;
                newTarget_Y =   _cp_Target_Y + _cp_Speed_Y * INSPECTOR_CONTROL_MOVE_TIME_STEP;
                time        =   INSPECTOR_CONTROL_MOVE_TIME_STEP;
                newTarget   =   true;

                //inspector is now moving
                _cp_TimeStamp = millis();
                _cp_Moving = true;
            }
            //check if inspector is continuous moving
            else
            {
                //calculate target
                newTarget_X =   _cp_Target_X + _cp_Speed_X * INSPECTOR_CONTROL_MOVE_TIME_STEP;
                newTarget_Y =   _cp_Target_Y + _cp_Speed_Y * INSPECTOR_CONTROL_MOVE_TIME_STEP;
                time        =   (INSPECTOR_CONTROL_MOVE_TIME_STEP - INSPECTOR_CONTROL_CALC_TIME_STEP) + INSPECTOR_CONTROL_MOVE_TIME_STEP;
                newTarget   =   true;

                //inspector is now moving
                _cp_TimeStamp = millis();
            }

            //check if new target ist available
            if(newTarget == true) {
                //calculate angles
                float   theta[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                if(!calculate_TCPAngles(newTarget_X, newTarget_Y, theta[0], theta[1])) {
                    return false;
                }

                //check if angles are in range
                if(_dynamixelManager->getServoDrive(0)->checkIfAngleIsInRage(theta[0]) &&
                   _dynamixelManager->getServoDrive(1)->checkIfAngleIsInRage(theta[1])) {

                    //update new target
                    _cp_Target_X    =   newTarget_X;
                    _cp_Target_Y    =   newTarget_Y;

                    //calculate angle theta3
                    theta[2] =   (theta[1] - theta[0]) + _orient_Theta1;
                    theta[3] =   _orient_Theta2;

                    //move inspector
                    if(!move_SynchronousToTargetAngles(time, theta, 4)) {
                        return false;
                    }
                }
            }
        }
        else {
            //inspector stops moving
            _cp_Moving = false;
        }
    }

    return true;
}

bool Control::move_Orientation() {
    //calculate time
    float time = static_cast<float>(millis() - _orient_TimeStamp) * 0.001f;

    //update tcp position
    float angle     =   -(_dynamixelManager->getServoDrive(0)->getAngleRel()) + (_dynamixelManager->getServoDrive(1)->getAngleRel()) -
                         (_dynamixelManager->getServoDrive(2)->getAngleRel());

    //update tcp pos
    _tcp_PosX       =   _cp_Target_X + INSPECTOR_L_4_MM * cosf(angle) + INSPECTOR_L_5_MM * cosf(angle) * cosf(_dynamixelManager->getServoDrive(3)->getAngleRel());
    _tcp_PosY       =   _cp_Target_Y + INSPECTOR_L_4_MM * sinf(angle) + INSPECTOR_L_5_MM * sinf(angle) * cosf(_dynamixelManager->getServoDrive(3)->getAngleRel());


    //check if blocking is enabled
    if(_orient_Block_Time != 0.0f) {
        //wait
        if(time >= _orient_Block_Time) {
            _orient_Block_Time  = 0.0f;
        }
        else {
            return true;
        }
    }

    //check if 35 ms have passed
    if(time > INSPECTOR_ORIENT_UPDATE_TIME) {

        //check if orientation servos are moved
        if(fabsf(_orient_Speed1) > 0.5f || fabsf(_orient_Speed2) > 0.5f) {
            //get angles of the other servo drives
            float theta1    =   _dynamixelManager->getServoDrive(0)->getAngleRel();
            float theta2    =   _dynamixelManager->getServoDrive(1)->getAngleRel();

            if(_orient_Move == false) {
                //calculate new offset
                _orient_Theta1  +=  _orient_Speed1 * INSPECTOR_ORIENT_UPDATE_TIME * 2.0f;
                _orient_Theta2  +=  _orient_Speed2 * INSPECTOR_ORIENT_UPDATE_TIME * 2.0f;
                time = INSPECTOR_ORIENT_UPDATE_TIME * 2.0f;
                _orient_Move = true;
            }
            else {
                //calculate new offset
                _orient_Theta1  +=  _orient_Speed1 * INSPECTOR_ORIENT_UPDATE_TIME;
                _orient_Theta2  +=  _orient_Speed2 * INSPECTOR_ORIENT_UPDATE_TIME;
                time = time + INSPECTOR_ORIENT_UPDATE_TIME * 2.0f;
            }

            //calculate new angle
            float theta3    =   theta2 - theta1 + _orient_Theta1;

            //range orient theta 1
            if(theta3 > 1.57f) {
                _orient_Theta1   =   1.57f + theta1 - theta2;
            }
            else if(theta3 < -1.57f) {
                _orient_Theta1   =   -1.57f + theta1 - theta2;
            }

            //range theta 4
            if(_orient_Theta2 > _dynamixelManager->getServoDrive(3)->getCCWLimitRel()) {
                _orient_Theta2 = _dynamixelManager->getServoDrive(3)->getCCWLimitRel();
            }
            else if(_orient_Theta2 < _dynamixelManager->getServoDrive(3)->getCWLimitRel()) {
                _orient_Theta2 = _dynamixelManager->getServoDrive(3)->getCWLimitRel();
            }

            //calculate angle again
            theta3    =   theta2 - theta1 + _orient_Theta1;

            //calculate delta
            float dTheta3   =   theta3 - _dynamixelManager->getServoDrive(2)->getAngleRel();

            //update orientation of second drive
            float dTheta4    =  _orient_Theta2 - _dynamixelManager->getServoDrive(3)->getAngleRel();


            float speed3    =   fabsf((dTheta3 / time) / (2.0f * PI)) * 60.0f * 0.9f;
            float speed4    =   fabsf((dTheta4 / time) / (2.0f * PI)) * 60.0f * 0.9f;

            //reset timer
            _orient_TimeStamp = millis();


            //check if moving
            if(_cp_Moving == false) {
                //update positions
                _dynamixelManager->moveToTargetAngle(2, theta3, speed3);
                _dynamixelManager->moveToTargetAngle(3, _orient_Theta2, speed4);
            }
        }
        else {
            _orient_Move = false;
        }


    }

    return true;
}

void Control::block_Orientation(const float& time) {
    //block orientation for the needed time
    _orient_Block_Time  =   time;
    _orient_TimeStamp   =   millis();
    _orient_Move        =   false;
}

void Control::getPosition(float* xPos, float* yPos) {
    *xPos = _cp_Target_X;
    *yPos = _cp_Target_Y;
}

void Control::getTCPPosition(float* xPos, float* yPos) {
    *xPos = _tcp_PosX;
    *yPos = _tcp_PosY;
}

void Control::checkCollision() {
    InspectorMath::Vector3 pos[2]   =   {InspectorMath::Vector3(_cp_Target_X, _cp_Target_Y, 0.0f),
                                         InspectorMath::Vector3(_tcp_PosX, _tcp_PosY, 0.0f)};

    //check positions
    for(uint8 i = 0; i < 2; i++) {
        float distX =   _planeX.distance(pos[i]);
        float distY =   _planeY.distance(pos[i]);

        //check collision
        if(distX <= 0.0f && distY <= 0.0f) {

            if(distY < distX) {
                if(_cp_Speed_X < 0.0f) {
                    _cp_Speed_X = 3.0f;
                }
            }
            else {
                if(_cp_Speed_Y < 0.0f) {
                    _cp_Speed_Y = 3.0f;
                }
            }
        }

        if(i == 1 && distX <= 40.0f && distY <= -220.0f) {
            if(_orient_Speed1 > 0.0f) {
                _orient_Speed1 = 0.0f;
            }
        }
    }



}

};
