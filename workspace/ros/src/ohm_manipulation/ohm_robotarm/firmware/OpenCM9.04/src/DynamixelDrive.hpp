/*
 * drive.hpp
 *
 *  Created on: 19.02.2016
 *      Author: feesma44884
 */

#ifndef DYNAMIXELDRIVE_HPP_
#define DYNAMIXELDRIVE_HPP_

/**
 * @class DynamixelDrive
 *
 * @brief Class representing a dynamixel drive
 */
struct DynamixelDrive
{
public:
  /**
   * @brief Standard constructor
   */
  DynamixelDrive() : _ID(0), _start_position(0), _moving_speed(0), _actual_position(0), _target_position(0), _target_position_old(0) {}

  /**
   * @brief Constructor
   * @param ID[in] Hardware id of the drive
   * @param start_position[in] Start position of the drive
   * @param moving_speed[in] Moving speed of the drive
   */
  DynamixelDrive(const uint8_t ID, const uint16_t start_position, const uint16_t moving_speed) :
    _ID(ID), _start_position(start_position), _moving_speed(moving_speed), _actual_position(start_position),
    _target_position(start_position), _target_position_old(start_position) {}

  const uint8_t   _ID;                  //!< Hardware ID of the drive

  const uint16_t  _start_position;      //!< Start position at startup
  uint16_t        _moving_speed;        //!< Speed of the drive

  int16_t         _actual_position;     //!< Actual position of the drive

  int16_t         _target_position;     //!< Target position of the drive
  int16_t         _target_position_old; //!< Old target position of the drive
};

#endif /* DYNAMIXELDRIVE_HPP_ */
