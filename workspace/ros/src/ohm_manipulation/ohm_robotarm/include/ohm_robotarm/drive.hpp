/*
 * drive.hpp
 *
 *  Created on: 01.03.2016
 *      Author: feesma44884
 */

#ifndef DRIVE_HPP_
#define DRIVE_HPP_

#include <ros/ros.h>
#include <ohm_robotarm/DriveState.h>

/***
 * @namespace ohm_robotarm
 *
 * @brief Namespace of the AutonOHM robotarm
 */
namespace ohm_robotarm
{

  /**
   * @namespace Drive
   *
   * @brief Namespace for representing drives/joint of the robotarm
   */
  namespace Drive
  {
    /**
     * @brief Drive types of the robotarm
     */
    enum Type {
      T_SIM_DRIVE        = 0,//!< T_SIM_DRIVE
      T_ELMO_DRIVE       = 1,//!< T_ELMO_DRIVE
      T_DYNAMIXEL_DRIVE  = 2 //!< T_DYNAMIXEL_DRIVE
    };

    /**
     * @brief Datafield position of data type on param server ( ID | Type | Hardware-ID ...)
     */
    enum DataField {
      DF_ID             = 0,//!< DF_ID
      DF_TYPE           = 1,//!< DF_TYPE
      DF_HARDWARE_ID    = 2,//!< DF_HARDWARE_ID
      DF_RESOLUTION     = 3,//!< DF_RESOLUTION
      DF_DIRECTION      = 4,//!< DF_DIRECTION
      DF_OFFSET         = 5 //!< DF_OFFSET
    };

    /**
     * @struct Data
     *
     * @brief Structure is saving data of a drive/joint
     */
    struct Data {
    public:

      /**
       * @brief Standard constructor of the drive data structure
       *
       * @param name[in] Name of the drive/joint
       * @param id[in] Unique id of the drive/joint
       * @param type[in] Type of the drive/joint
       * @param hardware_id[in] Hardware-ID of the drive/joint (CANopen Node-ID, Dynamixel-ID)
       * @param encoder_resolution[in] Resolution of the encoder
       * @param direction[in] Direction (+/- 1)
       * @param offset[in] Offset (zero position)
       * @param has_position_limits[in] True if drive has position limits
       * @param position_limit_min[in] Minimal possible position
       * @param position_limit_max[in] Maximal possible position
       * @param has_velocity_limit[in] True if drive has velocity limit
       * @param velocity_limit[in] Maximal velocity
       * @param has_acceleration_limit[in] True if drive has acceleration limit
       * @param acceleration_limit[in] Maximum acceleration
       */
      Data(const std::string& name, const unsigned int id, const Type& type, const unsigned int hardware_id,
                const unsigned int encoder_resolution, const int direction, const int offset,
                const bool has_position_limits, const double position_limit_min, const double position_limit_max,
                const bool has_velocity_limit, const double velocity_limit, const bool has_acceleration_limit, const double acceleration_limit);

      /**
       * @brief Drive definition
       */
      const std::string     _name;        //!< Unique name of the drive/joint
      const unsigned int    _id;          //!< Unique ID of the drive/joint
      const Type            _type;        //!< Type of the drive/joint
      const unsigned int    _hardware_id; //!< Hardware ID of the drive/joint (e.g. CANopen node ID, Dynamixel ID...)

      /**
       * @brief Drive configuration
       */
      const unsigned int    _encoder_resolution;  //!< Resolution of the drive encoder
      const int             _direction;           //!< Direction
      const int             _offset;              //!< Offset from 0 position

      /**
       * @brief Drive limits
       */
      const bool              _has_position_limits;     //!< If true drive/joint has position limits
      const double            _position_limit_max;      //!< Maximum possible position
      const double            _position_limit_min;      //!< Minimal possible position
      const bool              _has_velocity_limit;      //!< If true drive/joint has velocity limit
      const double            _velocity_limit;          //!< Maximum velocity
      const bool              _has_acceleration_limit;  //!< If true drive/joint has acceleration limits
      const double            _acceleration_limit;      //!< Acceleration limit
    };

    /**
     * @class Drive
     *
     * @brief This class is a parent class for all drives used by the robotarm (e.g. Dynamixel, Harmonic Drive (Elmo MC), simulated drives...)
     */
    class Drive {
    public:
      /**
       * @brief Constructor of drive/joint class (loads data from param server)
       *
       * @param node_handle[in] Node handle
       * @param name[in] Name of the drive/joint (e.g. joint_1)
       */
      Drive(ros::NodeHandle& node_handle, const std::string& name);

      /**
       * @brief Destructor of the class
       */
      virtual ~Drive(void);

      /**
       * @brief Converts an incremental to a radial value
       *
       * @param value[in] Value to convert
       *
       * @return value in radial
       */
      const double convert_inc_to_rad(const int value);

      /**
       * @brief Converts an radial to an incremental value
       *
       * @param value[in] Value to convert
       *
       * @return value in increments
       */
      const int convert_rad_to_inc(const double value);

      /**
       * @brief Get functions for getting data of the drive
       */
      const std::string&    get_name(void);
      const unsigned int    get_id(void);
      const Type&           get_type(void);
      const unsigned int    get_hardware_id(void);

      const unsigned int    get_encoder_resolution(void);
      const unsigned int    get_direction(void);
      const unsigned int    get_offset(void);

      const bool            get_has_position_limits(void);
      const double          get_position_limit_min(void);
      const double          get_position_limit_max(void);

      const bool            get_has_velocity_limit(void);
      const double          get_velocity_limit(void);

      const bool            get_has_acceleration_limit(void);
      const double          get_acceleration_limit(void);

      const DriveState&     get_tx_state(void);
      const DriveState&     get_rx_state(void);

      const bool            get_is_initialized(void);

    protected:

      /**
       * @brief Joint data
       */
      std::shared_ptr<Data>   _data;                //!< Data of the drive/joint

      /**
       * @brief State/Command of the drives
       */
      ros::Subscriber         _state_subscriber;    //!< Subscriber for the state of the drive/joint
      ros::Publisher          _state_publisher;     //!< Publisher for the state of the drive/joint
      ros::Timer              _update_timer;        //!< Timer for publishing drive/joint state

      /**
       * @brief States of the drive
       */
      DriveState              _tx_state;                //!< State to transmit (position, velocity, acceleration)
      DriveState              _rx_state;                //!< Received state (position, velocity, acceleration)

      bool                    _is_initialized;          //!< Saves true if class is initialized
    };
  };
};

#endif /* DRIVE_HPP_ */
