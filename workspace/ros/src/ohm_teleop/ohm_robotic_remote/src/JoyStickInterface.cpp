/*
 * JoyStickActionInterface.cpp
 *
 *  Created on: 15.04.2015
 *      Author: chris
 */

#include "JoyStickInterface.h"

JoyStickInterface* JoyStickInterface::_instance = 0;

JoyStickInterface* JoyStickInterface::getInstance(void)
{
   if(!_instance)      _instance = new JoyStickInterface();
   return _instance;
}

JoyStickInterface::JoyStickInterface(void)
: _nh(NULL)
{

}

JoyStickInterface::~JoyStickInterface(void)
{
   if(_instance)
   {
      delete _instance;
      _instance = 0;
   }
}

void JoyStickInterface::setNodeHandle(ros::NodeHandle* nh)
{
   _nh = nh;



   // init subscribers
   _joyActionSub = _nh->subscribe("/joy_action", 1, &JoyStickInterface::joyActionCallback, this);

}



void JoyStickInterface::joyActionCallback(const ohm_teleop_msgs::Action& msg)
{
   if(msg.status == msg.CHANGE_NS)
   {
      std::cout << "switch ns" << std::endl;
      emit switchNamespace();
   }
   else if (msg.status == msg.CHANGE_VIEWER) {
      std::cout << "switch viewer" << std::endl;
      emit nextImageInViewer();
   }
}
