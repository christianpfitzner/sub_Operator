/*
 * JoyMapper.cpp
 *
 *  Created on: 12.02.2015
 *      Author: chris
 */

#include "../../ohm_joy/src/JoyMapper.h"
#include <QSettings>


JoyMapper::JoyMapper(void)
{
   _isInitialized = false;
   _throttle      = 1.0;
   _v_max         = 1.2;
   _driverReverse = false;
}

JoyMapper::~JoyMapper(void)
{

}

void JoyMapper::setConfigFile(std::string filepath)
{
   _filepath = QString::fromStdString(filepath);

   QSettings settings(_filepath, QSettings::NativeFormat);

   unsigned int i=0 ;
   settings.beginGroup("axis");
      settings.beginGroup("Forward");
         _axis_map.v_forward_idx      = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Backwards");
         _axis_map.v_backward_idx     = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Left-Right");
         _axis_map.yaw_idx            = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Camera-Up-Down");
         _axis_map.cam_up_down_idx    = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Camera-Left-Right");
         _axis_map.cam_left_right_idx = settings.value("value").toInt(); i++;
      settings.endGroup();

      // manipulator
      settings.beginGroup("Manipulator: TCP Up-Down");
         _axis_map.manip_2D_up_down_idx = settings.value("value").toInt();   i++;
      settings.endGroup();
      settings.beginGroup("Manipulator: TCP Forward");
         _axis_map.manip_tcp_forward_idx = settings.value("value").toInt();  i++;
      settings.endGroup();
      settings.beginGroup("Manipulator: TCP Backward");
         _axis_map.manip_tcp_backward_idx = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Manipulator: Orientation Left-Right");
         _axis_map.manip_left_right_idx  = settings.value("value").toInt();  i++;
      settings.endGroup();
      settings.beginGroup("Manipulator: Orientation Up-Down");
         _axis_map.manip_up_down_idx      = settings.value("value").toInt(); i++;
      settings.endGroup();
   settings.endGroup();

   i=0;
   settings.beginGroup("buttons");
      settings.beginGroup("Acknowledged");
         _button_map.acknowledget_idx  = settings.value("value").toInt();   i++;
      settings.endGroup();
      settings.beginGroup("Decline");
         _button_map.decline_idx       = settings.value("value").toInt();   i++;
      settings.endGroup();
      settings.beginGroup("Switch-Autonomy-Teleop");
         _button_map.tele_autonomy_idx = settings.value("value").toInt();   i++;
      settings.endGroup();
      settings.beginGroup("Reset-Camera");
         _button_map.reset_cam_idx = settings.value("value").toInt();       i++;
      settings.endGroup();
      settings.beginGroup("Change-Namespace");
         _button_map.change_namespace_idx = settings.value("value").toInt();i++;
      settings.endGroup();
      settings.beginGroup("Change-Viewer");
         _button_map.change_viewer_idx = settings.value("value").toInt();   i++;
      settings.endGroup();

      /*
       * Manipulator buttons
       */
      settings.beginGroup("Manipulator: Move Out");
         _button_map.manip_move_out_idx= settings.value("value").toInt();   i++;
      settings.endGroup();
      settings.beginGroup("Manipulator: Move Home");
         _button_map.manip_move_home_idx = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Manipulator: Drive Slowly");
         _button_map.manip_base_slow     = settings.value("value").toInt(); i++;
      settings.endGroup();

      /*
       * Flipper buttons
       */
      settings.beginGroup("Flipper: Select Front");
         _button_map.flipper_select_front= settings.value("value").toInt();   i++;
      settings.endGroup();
      settings.beginGroup("Flipper: Select Back");
         _button_map.flipper_select_back = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Flipper: Select Both");
         _button_map.flipper_select_both     = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Flipper: up");
         _button_map.flipper_up              = settings.value("value").toInt(); i++;
      settings.endGroup();
      settings.beginGroup("Flipper: down");
         _button_map.flipper_down            = settings.value("value").toInt(); i++;
      settings.endGroup();
   settings.endGroup();
}

void JoyMapper::setJoyMsg(const sensor_msgs::Joy& joy)
{
   _joy = joy;
}

void JoyMapper::map(void)
{
   // get velocitys
   if(_isInitialized)
   {
      this->generateTwistMessages();
      this->generateTeleopActionMessages();
      this->generateSensorHeadMessages();
      this->generateInspectorMessages();
      this->generateFlipperMessages();
   }
   else
   {
//      std::cout << "is not initialized" << std::endl;
      if(_joy.axes[_axis_map.v_forward_idx] ==  1.0 && _joy.axes[_axis_map.v_backward_idx] == 1.0)
         _isInitialized = true;
      if(_joy.axes[_axis_map.v_forward_idx] == -1.0 && _joy.axes[_axis_map.v_backward_idx] == -1.0)
         _isInitialized = true;
   }
}


void JoyMapper::generateTwistMessages(void)
{
   float forward = 0.0;
   float angular = 0.0;

   if(_axis_map.v_backward_idx == _axis_map.v_forward_idx) {
      forward  = -1* _joy.axes[_axis_map.v_forward_idx];
   }
   else {
      forward  = -1* (_joy.axes[_axis_map.v_forward_idx] - _joy.axes[_axis_map.v_backward_idx]);
   }


   angular  = _joy.axes[_axis_map.yaw_idx];


   _twist.linear.x  = (forward/2.0)*(std::fabs(forward)/2.0)  * _throttle * 2;
   _twist.angular.z = (angular)*(std::fabs(angular))  * _throttle;  //_joy.axes[_axis_map.yaw_idx] * 8.0      * _throttle;

   if(_twist.linear.x >   _v_max)
      _twist.linear.x =   _v_max;
   if(_twist.linear.x <  -_v_max)
      _twist.linear.x =  -_v_max;

//   if(_twist.angular.z >  10.0*_v_max)
//      _twist.angular.z =  10.0*_v_max;
//   if(_twist.angular.z <  10.0*-_v_max)
//      _twist.angular.z =  10.0*-_v_max;


   // this is mandatory for reverse driving
   if(_driverReverse) _twist.linear.x  = _twist.linear.x * (-1.0);
}


void JoyMapper::generateTeleopActionMessages(void)
{
   bool actionChanged = false;

   _sensorHead.home   = false;

   for(unsigned int i=0 ; i<_joy.buttons.size() ; i++)
   {
      if(_joy.buttons[i] == 1)
      {
         if(i ==  _button_map.acknowledget_idx) {
            _action.status = _action.CONFIRM;
//            ROS_INFO("confirm pressed");
            actionChanged = true;
         }
         else if(i == _button_map.decline_idx) {
            _action.status = _action.CANCEL;
//            ROS_INFO("declined pressed");
            actionChanged  = true;
         }
         else if(i == _button_map.tele_autonomy_idx) {
            _action.status = _action.AUTO_MAN;
            actionChanged  = true;
         }
//         else if(i == _button_map.manip_move_out_idx) {
//            _action.status = _action.
//            actionChanged  = true;
//         }
         else if(i == _button_map.manip_base_slow) {
            _action.status = _action.SWITCH_TELE_ASS;
            actionChanged  = true;
         }
         else if(i == _button_map.reset_cam_idx) {
            _sensorHead.home = true;
            actionChanged    = true;
         }
         else if(i == _button_map.change_namespace_idx) {
            _action.status = _action.CHANGE_NS;
            actionChanged  = true;
         }
         else if(i == _button_map.change_viewer_idx) {
            _action.status = _action.CHANGE_VIEWER;
            actionChanged  = true;
         }
      }
   }
   if(!actionChanged) _action.status = _action.DEFAULT;
}


void JoyMapper::generateSensorHeadMessages(void)
{
   _sensorHead.pitch             =  -_joy.axes[_axis_map.cam_up_down_idx]*0.6;
   _sensorHead.yaw               =  _joy.axes[_axis_map.cam_left_right_idx]*0.6;
}



void JoyMapper::generateInspectorMessages(void)
{
   /*
    * speed of tcp
    */
   float manip_forward;

   if(_axis_map.manip_tcp_forward_idx == _axis_map.manip_tcp_backward_idx) {
      manip_forward  = -1* _joy.axes[_axis_map.manip_tcp_forward_idx];
   }
   else {
      manip_forward = -1* (_joy.axes[_axis_map.manip_tcp_forward_idx] - _joy.axes[_axis_map.manip_tcp_backward_idx]);
   }

   manip_forward = (manip_forward)*(std::fabs(manip_forward))/4.0;


   // set up controlls for manipulator
   static unsigned int seq    = 0;
   _inspector.header.frame_id = "inspector_base";
   _inspector.header.seq      = seq++;
   _inspector.header.stamp    = ros::Time::now();

   _inspector.speed2D[0]      = _joy.axes[_axis_map.manip_2D_up_down_idx];
   _inspector.speed2D[1]      = _joy.axes[_axis_map.manip_2D_forward_backward_idx];

   _inspector.speedAxis[0]    = _joy.axes[_axis_map.manip_up_down_idx];
   _inspector.speedAxis[1]    = _joy.axes[_axis_map.manip_left_right_idx];

   _inspector.speedTCP        = manip_forward;

   _inspector.moveHome        = _joy.buttons[_button_map.manip_move_out_idx];
   _inspector.movePark        = _joy.buttons[_button_map.manip_move_home_idx];
}


void JoyMapper::generateFlipperMessages(void)
{
  std::cout << __PRETTY_FUNCTION__ << "helllo" << std::endl;
   const float max_speed = 3000.0f;

   ohm_teleop_msgs::FlipperSpeed flipper;

   float speed = 0.0;


   if(_joy.buttons[_button_map.flipper_up])
      speed =  max_speed;
   if(_joy.buttons[_button_map.flipper_down])
      speed = -max_speed;

   if(_joy.buttons[_button_map.flipper_select_front] || _joy.buttons[_button_map.flipper_select_both])
   {
      flipper.front_left  = speed;
      flipper.front_right = speed;
   }
   if(_joy.buttons[_button_map.flipper_select_back] || _joy.buttons[_button_map.flipper_select_both])
   {
      flipper.back_left  = speed;
      flipper.back_right = speed;
   }

   _flipper_speed = flipper;
}

