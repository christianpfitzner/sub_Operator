/*
 * JoyStickActionInterface.h
 *
 *  Created on: 15.04.2015
 *      Author: chris
 */

#ifndef OHM_ROBOTIC_REMOTE_SRC_JOYSTICKINTERFACE_H_
#define OHM_ROBOTIC_REMOTE_SRC_JOYSTICKINTERFACE_H_

// ros includes
#include <ros/ros.h>

#include <QObject>

// ohm-teleop includes
#include "ohm_teleop_msgs/Action.h"

/**
 * @class   JoyStickAction
 * @author  Christian Pfitzner
 * @date    2015-04-15
 */
class JoyStickInterface : public QObject
{
   Q_OBJECT

public:
   static JoyStickInterface* getInstance(void);

   /**
    * Default destructor
    */
   virtual ~JoyStickInterface();

   /**
    * Function to set node handle
    * @param nh
    */
   void setNodeHandle(ros::NodeHandle* nh);

signals:
   void switchNamespace(void);
   void nextImageInViewer(void);

private:
   /**
    * private constructor for singleton pattern
    */
   JoyStickInterface(void);
   /**
    * Dummy for copy constructor for joystick interface
    * @param
    */
   JoyStickInterface(const JoyStickInterface& ) {}
   /**
    * Callback function for joy action message
    * @param msg
    */
   void joyActionCallback(const ohm_teleop_msgs::Action& msg);



   static JoyStickInterface* _instance;


   ros::NodeHandle* _nh;
   ros::Subscriber  _joyActionSub;

};


#endif /* OHM_ROBOTIC_REMOTE_SRC_JOYSTICKINTERFACE_H_ */
