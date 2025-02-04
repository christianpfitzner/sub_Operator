/*
 * dynamixel_manager.h
 *
 *  Created on: 05.03.2015
 *      Author: feesma44884
 */

#ifndef OHM_INSPECTOR_FIRMWARE_SRC_DYNAMIXEL_MANAGER_H_
#define OHM_INSPECTOR_FIRMWARE_SRC_DYNAMIXEL_MANAGER_H_

#include "definitions.h"
#include "core/inc/wirish.h"
#include "core/inc/Dynamixel.h"

/**
 * Definitions for dynamixel manager
 */
#define DM_MAX_NUM_ERRORS   10

/**
 * Macros for dynamixel manager
 */
#define DM_DYN_TO_PI(x)     (x) * (0.001533980788f)
#define DM_PI_TO_DYN(x)     (x) * (651.8986469f)
#define DM_SPEED_TO_DYN(x)  (x) * (8.7719298f)

/**
 * Dynamixel Manager namespace contents classes and functions to control dynamixel drives with
 * the OpenCM board.
 */
namespace DynamixelManager
{

/**
 * Enumeration of the Dynamixel models
 */
enum Model
{
    NONE  =  0x0000,//!< NONE
    MX28  =  0x001D,//!< Model: MX28
    MX64  =  0x0136,//!< Model: MX64
    MX106 =  0x0140 //!< Model: MX106
};

/**
 * This class represents a dynamixel servo drive and contains all important data of it.
 */
class ServoDrive
{
public:

    /**
     * Constructor. Sets all parameters to zero.
     */
    ServoDrive();

    /**
     * Constructor. Sets needed to set all important params.
     * @param id[in] ID of the dynamixel servo
     * @param model[in] Model of the dynamixel servo
     * @param middAngle[in][0 - 4096] Reference angle where the relative angle is 0
     * @param cwLimit[in][0 - 4096] Position limit in clockwise direction
     * @param ccwLimit[in][0 - 4096] Position limit in counter clockwise direction
     * @param speedLimitRPM[0.0f - 112.0f] Speed limit in rounds per minute
     */
    ServoDrive(const uint8& id, const Model& model, const int16& middAngle, const int16& cwLimit,
               const int16& ccwLimit, const float& speedLimitRPM);

    /**
     * Updates the current angle of the servo
     *
     * @param newAngleDyn[in][0 - 4096] Current angle of the dynamixel drives
     */
    void updateAngle(const int16& newAngleDyn);

    /**
     * Converts a relative angle position to an absolute angle position
     *
     * @param angleRel[in][+/- 0.0f * pi]
     *
     * @return absolute angle [0 - 4096]
     */
    uint16 convertRelToAbsAngle(const float& angleRel);

    /**
     * Converts a speed from [rpm] into the dynamixel speed unit system [1 - 1023]
     *
     * @param speed[in][RPM]
     *
     * @return absolute speed [1 - 1023]
     */
    uint16 convertSpeed(const float& speed);


    /**
     * Returns the ID of the dynamixel servo
     *
     * @return _id
     */
    const uint8&    getID()     {return _id;}

    /**
     * Returns the model of the dynamixel servo
     *
     * @return _model
     */
    const Model&    getModel()  {return _model;}

    /**
     * Returns the absolute position of the dynamixel servo
     *
     * @return[0 - 4096] _angleDyn
     */
    const int16&    getAngleDyn()   {return _angleDyn;}

    /**
     * Returns the relative position of the dynamixel servo
     *
     * @return[+/- * pi] _angleRel
     */
    const float&    getAngleRel()   {return _angleRel;}

    /**
     * Returns the absolute position limit in clockwise direction
     *
     * @return[0 - 4096] _cwLimitDyn
     */
    const uint16   getCWLimitDyn() {return static_cast<uint16>(_cwLimitDyn);}

    /**
     * Returns the absolute position limit in counter clockwise direction
     *
     * @return[0 - 4096] _ccwLimitDyn
     */
   const uint16    getCCWLimitDyn() {return static_cast<uint16>(_ccwLimitDyn);}

   /**
    * Returns the relative position limit in clockwise direction
    *
    * @return[+/- * pi] _cwLimitRel
    */
   const float    getCWLimitRel()   {return _cwLimitRel;}

   /**
   * Returns the relative position limit in counter clockwise direction
   *
   * @return[+/- * pi] _ccwLimitRel
   */
  const float    getCCWLimitRel()   {return _ccwLimitRel;}

  /**
   * Returns the setup speed limit of the dynamixel drive
   *
   * @return _speedLimitRPM
   */
  const float&  getMaxSpeedRPM()       {return _speedLimitRPM;}

  /**
   * Checks if the angle is in range
   *
   * @param angle[in][+/- * pi] Relative angle position
   *
   * @return true if angle is in range [cw/ccw limit]
   */
  const bool    checkIfAngleIsInRage(const float& angle);

  uint8    _errorCnt;       /**< Saves how often an error occurred to this dynamixel drive */
  uint8    _errorCheck;     /**< Needed that error check is not performed every time */

private:
    uint8   _id;            /**< ID of the dynamixel drive */
    Model   _model;         /**< Model of the dynamixel drive */

    /**
     * @brief: Description of variables
     *
     * @_[...]Dyn: These variables saves the values which can directly be transmitted to the dynamixel drive
     * @_[...]Abs: These variables saves the values in absolute coordinates
     * @_[...]Rel: These variable saves the values in relative coordinate referred to the middle position
     */

    int16   _cwLimitDyn;        /**<Limit of movement in clockwise direction (absolute value [0 - 4096]) */
    float   _cwLimitRel;        /**<Limit of movement in clockwise direction (relative value [+/- 0.0 * pi])*/

    int16   _ccwLimitDyn;       /**<Limit of movement in counter clockwise direction (absolute value [0 - 4096])*/
    float   _ccwLimitRel;       /**<Limit of movement in counter clockwise direction (relative value [+/- 0.0 * pi])*/

    int16   _speedLimitDyn;     /**<Limit of speed (absolute value) [1 - 1023]*/
    float   _speedLimitRPM;     /**<Limit of speed (relative value) [0.114 - 117.07 RPM]*/

    int16   _middAngleDyn;      /**<Absolute middle angle -> reference for relative positions [0 - 4096]*/
    int16   _angleDyn;          /**<Absolute angle of the dynamixel drive [0 - 4096]*/
    float   _angleRel;          /**<Relative angle of the dynamixel drives [+/- 0.0 * pi]*/
};

/**
 * Manages all dynamixel drives.
 * Enables special features like synchronous movement of the dynamixel drives.
 *
 */
class Manager
{
public:

    /**
     * Constructor
     */
    Manager();

    /**
     * Registers the servo drives to the manager
     *
     * @param drives[in] Array containing the drive information
     * @param numDrives[in] Size of the array
     *
     */
    void registerDrives(const ServoDrive* drives, const uint8 numDrives);

    /**
     * Desctructor: Releases all allocated memory
     */
    ~Manager();

    /**
     * Releases all allocated memory.
     */
    void release();

    /**
     * Enables movement of dynamixel drives
     *
     * @return true if run state was initialized successfully
     */
    bool initRunState();

    /**
     * Disables movement of dynamixel drives -> stops every movement
     *
     */
    void exitRunState();

    /**
     * Moves the drive to the target angle
     *
     * @param id[in] Id of the servo drive in the !array! (_servoDrives)
     * @param targetAngleRel[in][+/- * pi] Target angle
     *
     * @return true if command was sent successfully to drives
     */
    bool moveToTargetAngle(const uint8& id, const float& targetAngleRel);

    /**
     * Moves the drive to the target angle with the target speed
     *
     * @param id[in] Id of the servo drive in the !array! (_servoDrives)
     * @param targetAngleRel[in][+/- * pi] Target angle
     * @param targetSpeed[in][RPM] Target speed
     * @return true if command was sent successfully to drives
     */
    bool moveToTargetAngle(const uint8& id, const float& targetAngleRel, const float& targetSpeed);

    /**
     * Moves the drive simultaneously to the target position
     *
     * @param maxID[in] Max ID in the array of _servoDrives
     * @param targetAnglesRel[in][+/- * pi] Array with the target angles
     * @param targetSpeedsRPM[in][RPM] Array with the target speeds
     * @return true if command was sent successfully to drives
     */
    bool moveServoDrivesSimultanous(const uint8& maxID, const float targetAnglesRel[], const float targetSpeedsRPM[]);

    /**
     * Stops all dynamixel drives at the current position.
     */
    void stopMovement();

    /**
     * Returns a pointer to the hardware timer used to update the dynamixel drive positions
     *
     * @return Instance to the hardware timer
     */
    HardwareTimer*  getTimer()      {return &_timer;}

    /**
     * Returns the current run state
     *
     * @return true: run state active false: run state inactive
     */
    const volatile bool& getRunState()  {return _inRunState;}

    /**
     * Returns a pointer to the wanted drive
     *
     * @param id[in] Id of the sero drives in the !array! (_servoDrives)
     * @return
     */
    ServoDrive*     getServoDrive(const uint8& id)  {return &_servoDrives[id];}

    /**
     * Returns the number of the servo drives registered
     *
     * @return _numServoDrives
     */
    const uint8&    getNumServoDrives()             {return _numServoDrives;}

    /**
     * Sets the punch value of the selected drive
     *
     * @param id[in] Id of the sero drives in the !array! (_servoDrives)
     * @param punch[in]: Value of punch
     */
    void            setPunch(const uint8& id, const uint16& punch);

    /**
     * This function is called from the callback of the hardware timer
     */
    void callback_timer();

private:

    /**
     * Updates the current angle position of the drives
     *
     * @return true if all angles are valid
     */
    bool updateAngles();


    HardwareTimer   _timer;          /**<Timer for updating the position of the dynamixel drives*/
    Dynamixel       _dynamixelBus;   /**<Communication bus*/
    ServoDrive*     _servoDrives;    /**<Array which saves all dynamixel drives*/
    uint8           _numServoDrives; /**<Saves the number of servo drives*/
    bool            _stopActive;     /**<Saves true when dynamixels are stopped*/

    volatile uint8  _errorCnt;       /**<Saves how much errors occure*/
    volatile bool   _inRunState;     /**<Saves true when the manager is in run state*/

    volatile uint8  _updateID;      /**<Saves the ID of the drive which will be updatet next*/
};


} /*namespace DynamixelManager*/



#endif /* OHM_INSPECTOR_FIRMWARE_SRC_DYNAMIXEL_MANAGER_H_ */
