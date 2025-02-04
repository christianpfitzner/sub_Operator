/*
 * JoyMapper.h
 *
 *  Created on: 12.02.2015
 *      Author: chris
 */

#ifndef OHM_JOY_SRC_JOYMAPPER_H_
#define OHM_JOY_SRC_JOYMAPPER_H_

#include <QString>
#include <string>

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <ohm_actors_msgs/SensorHeadJoy.h>
#include <ohm_actors_msgs/Command.h>

#include <ohm_teleop_msgs/Action.h>
#include <ohm_teleop_msgs/FlipperSpeed.h>


/**
 * @class   JoyMapper
 * @author  Christian Pfitzner
 * @date    2015-03-10
 *
 * @brief   mapper for joystick messages towards joy and action
 */
class JoyMapper
{
public:
   /**
    * Default constructor
    *
    */
   JoyMapper(void);
   /**
    * Default destructor
    */
   virtual ~JoyMapper(void);

   // SETTERS
   /**
    * Function to set config file
    * @param filepath
    */
   void setConfigFile(std::string filepath);
   /**
    * Function to set joy message
    * @param joy
    */
   void setJoyMsg(const sensor_msgs::Joy& joy);
   /**
    * Function to set maximum velocity in percent
    * @param scale
    */
   void setMaxVelocityThrottle(const double& throttle)
   {
      _throttle = throttle;
   }

   void setVMax(const float& vmax)
   {
      _v_max = vmax;
   }


   void setOrientation(bool driveReverse)
   {
      _driverReverse = driveReverse;
   }

   // PROCESSING
   /**
    * Function to be called for mappinig
    */
   void map(void);

   // GETTERS
   /**
    * Function to return twist msg
    * @return
    */
   geometry_msgs::Twist getTwist(void) const
   {
      return _twist;
   }
   /**
    * Function to get joy action from mapper
    * @return
    */
   ohm_teleop_msgs::Action getTeleopAction(void) const
   {
      return _action;
   }
   /**
    * @fn   Function to get sensor head message from joy
    * @return
    */
   ohm_actors_msgs::SensorHeadJoy getSensorHeadJoy(void) const
   {
      return _sensorHead;
   }
   /**
    * @fn   Function to get inspector message from joy
    * @return
    */
   ohm_actors_msgs::Command getInspector(void) const
   {
      return _inspector;
   }

   ohm_teleop_msgs::FlipperSpeed getFlipperSpeed(void) const
   {
      return _flipper_speed;
   }

   float getVMax(void) const
   {
      return _v_max;
   }

private:
   /**
    * Function to generate twist messages
    */
   void generateTwistMessages(void);
   /**
    * Function to generate action messages
    */
   void generateTeleopActionMessages(void);
   /**
    * Function to generate sensor head messages
    */
   void generateSensorHeadMessages(void);
   /**
    * Function to generate inspector messages
    */
   void generateInspectorMessages(void);
   /**
    * Function to generate flipper message
    */
   void generateFlipperMessages(void);

   /**
    * @struct AxisMapping
    */
   struct AxisMapping
   {
      unsigned int v_forward_idx;
      unsigned int v_backward_idx;
      unsigned int yaw_idx;
      unsigned int cam_up_down_idx;
      unsigned int cam_left_right_idx;

      // manipulator
      unsigned int manip_2D_up_down_idx;
      unsigned int manip_2D_forward_backward_idx;

      unsigned int manip_tcp_forward_idx;
      unsigned int manip_tcp_backward_idx;

      // orientation
      unsigned int manip_left_right_idx;
      unsigned int manip_up_down_idx;
   };
   /**
    * @struct ButtonMapping
    */
   struct ButtonMapping
   {
      unsigned int acknowledget_idx;
      unsigned int decline_idx;
      unsigned int tele_autonomy_idx;
      unsigned int reset_cam_idx;

      unsigned int change_namespace_idx;
      unsigned int change_viewer_idx;

      unsigned int manip_move_out_idx;
      unsigned int manip_move_home_idx;
      unsigned int manip_base_slow;

      unsigned int flipper_select_front;
      unsigned int flipper_select_back;
      unsigned int flipper_select_both;

      unsigned int flipper_up;
      unsigned int flipper_down;
   };

   QString                          _filepath;

   sensor_msgs::Joy                 _joy;
   geometry_msgs::Twist             _twist;
   ohm_actors_msgs::SensorHeadJoy   _sensorHead;
   ohm_actors_msgs::Command         _inspector;
   ohm_teleop_msgs::Action          _action;
   ohm_teleop_msgs::FlipperSpeed    _flipper_speed;

   bool _setHome;

   float _v_max;

   bool _isInitialized;

   bool _driverReverse;

   AxisMapping _axis_map;
   ButtonMapping _button_map;

   double _throttle;
};

#endif /* OHM_JOY_SRC_JOYMAPPER_H_ */
