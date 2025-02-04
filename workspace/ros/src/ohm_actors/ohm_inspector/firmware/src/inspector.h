/*
 * inspector.h
 *
 *  Created on: 06.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_FIRMWARE_SRC_INSPECTOR_H_
#define OHM_INSPECTOR_FIRMWARE_SRC_INSPECTOR_H_

#include <math.h>

#include "definitions.h"
#include "core/inc/wirish.h"
#include "dynamixel_manager.h"
#include "InspectorMath.h"

/**
 * Controls the movement of the Inspector.
 * This class reflects the kinematic behavior of the inspector.
 */
namespace Inspector
{

/**
 * Dimensions of the inspector arm
 */
struct Dimension
{
    /**
     * Constructor -> Resets all values
     */
    Dimension();

    /**
     * Constructor -> Configs values
     *
     * @param length_0[in]
     * @param length_1[in]
     * @param length_2[in]
     * @param length_3[in]
     */
    Dimension(const float& length_0, const float& length_1, const float& length_2, const float& length_3);


    //Lengths
    float   _length_0;      /**<Length from base to the first servo drive*/
    float   _length_1;      /**<Length from the first servo drive to the assembly point of the second*/
    float   _length_2;      /**<Length from the assembly point to the pivot axis of the second servo drive*/
    float   _length_3;      /**<Length from the second servo drive to the assembly point of the third drive*/

    //Constant values
    float   _length_3_SQ;   /**<Length 3²*/
    float   _length_12;     /**<Length between pivot axis of first servo drive and pivot axis of second*/
    float   _length_12_SQ;  /**<Square of the Length*/
    float   _angle_12;      /**<angle between l1 and endpoint of l2*/
    float   _maxDistance;   /**<Maximum distance the inspector can reach, referred to the pivot point of first servo drive*/
};

/**
 * Controls the inspector behavior.
 *
 */
class Control
{
public:

    /**
     * Default constructor
     */
    Control();

    /**
     * Constructor: Initializes control class
     *
     * @param dynMan[in] Pointer to the instance of the dynamixel manager
     * @param dimensions[in] Dimensions of the inspector
     */
    Control(DynamixelManager::Manager* dynMan, const Dimension& dimensions);

    /**
     * Sets the distance of the plane from inspector base
     *
     * @param rangeX[in]
     * @param rangeY[in]
     */
    void setCollisionFactors(const float& rangeX, const float rangeY);


    /**
     * Runs the control class.
     * Moves the inspection arm with continous path methode.
     *
     * @param speed_X[in] Speed FWD/BWD (X - Direction)
     * @param speed_Y[in] Speed Up/Down (Y - Direction)
     * @param speed_A[in] Speed for first orientation drive
     * @param speed_B[in] Speed for second orientation drive
     *
     * @return true if function runs sucessfully
     */
    bool run(const float& speed_X, const float& speed_Y, const float& speed_A, const float& speed_B);

    /**
     * Move the inspector to his home position.
     */
    void move_ToHomePosition();

    /**
     * Moves the inspector to his park position.
     */
    void move_ToParkPosition();

    /**
     * Sets the orientation to the current position of the dynamixels
     */
    void reset_Orientation();


    /**
     * Returns the position of the third axis of the actuator
     *
     * @param xPos[out] x position
     * @param yPos[out] y position
     */
    void getPosition(float* xPos, float* yPos);

    /**
     * Returns the position of the TCP where the camera is mounted to
     *
     * @param xPos[out] x position
     * @param yPos[out] y position
     */
    void getTCPPosition(float* xPos, float* yPos);

private:

    /**
     * Calculates the current tool-center-point position of the inspector related to the angles.
     *
     * @param theta_1[in] Angle of the first drive
     * @param theta_2[in] Angle of the second drive
     * @param position_X[out] X - Position of the TCP
     * @param position_Y[out] Y - Position of the TCP
     */
    void calculate_TCPPosition(const float& theta_1, const float& theta_2, float& position_X, float& position_Y);

    /**
     * Calculates the inverse kinmatic.
     * Calculates the angles related to the position in x and y direction.
     *
     * @param position_X[in] Position in x direction
     * @param position_Y[in] Position in y direction
     * @param theta_1[out] Angle of the first drive
     * @param theta_2[out] Angle of the second drive
     *
     * @return true if position is reachable
     */
    bool calculate_TCPAngles(const float& position_X, const float& position_Y, float& theta_1, float& theta_2);


    /**
     * Moves the drives synchronous to the target angles in the given time.
     *
     * @param time[in] Time to reach the target angles
     * @param targetAngles[in] Array with the target angles
     * @param numAngles[in] Size of the array
     *
     * @return true if movement command was send successfully
     */
    bool move_SynchronousToTargetAngles(const float& time, const float* targetAngles, const uint8& numAngles);

    /**
     * Calculates the angles to move the inspector on a continous path (straight line)
     *
     * @return true if movement is possible
     */
    bool move_ContinuousPath();

    /**
     * Moves the last two drives to change orientation of the tool-center-point
     *
     * @return true if movement is possible
     */
    bool move_Orientation();

    /**
     * Blocks the function move_Orientation for time
     *
     * @param time[in] Time in seconds to block move_Orientation
     */
    void block_Orientation(const float& time);

    /**
     * Checks if inspector moves to invalid position and reduces speed if necessary
     */
    void checkCollision();

    Dimension                       _dimensions;        /**<Dimensions of the inspector*/
    DynamixelManager::Manager*      _dynamixelManager;  /**<Pointer to the Dynamixel Manager instance*/

    long                            _cp_TimeStamp;      /**<Time stamp for continuous path*/
    float                           _cp_TimeStep;       /**<Time step*/
    float                           _cp_NewTimeStep;    /**<New time step*/
    bool                            _cp_Moving;         /**<Saves the state of the movement*/
    float                           _cp_Target_X;       /**<Saves the target x - position*/
    float                           _cp_Target_Y;       /**<Saves the target y - position*/
    float                           _cp_Speed_X;        /**<Speed in x direction*/
    float                           _cp_Speed_Y;        /**<Speed in y direction*/

    float                           _orient_Block_Time; /**<Saves the time the orientation is blocked*/
    long                            _orient_TimeStamp;  /**<Time stamp for updating position*/
    float                           _orient_Theta1;     /**<Offset of the third servo drive -> first for orientation*/
    float                           _orient_Theta2;     /**<Offset of the fourth servo drive -> second orientation*/
    float                           _orient_Speed1;     /**<Speed for orientate first orientation servo*/
    float                           _orient_Speed2;     /**<Speed for orientate second orientation servo*/
    bool                            _orient_Move;       /**<Saves true if orientation servo drives are moving*/

    float                           _tcp_PosX;          /**<X position of the tool center point*/
    float                           _tcp_PosY;          /**<Y position of the tool center point*/

    InspectorMath::Plane            _planeX;            /**<Plane in X direction*/
    InspectorMath::Plane            _planeY;            /**<Plane in Y direction*/

};

};





#endif /* OHM_INSPECTOR_FIRMWARE_SRC_INSPECTOR_H_ */
