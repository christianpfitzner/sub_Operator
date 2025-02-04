/*
 * VictimNotificationDialog.h
 *
 *  Created on: 16.04.2015
 *      Author: chris
 */

#ifndef OHM_ROBOTIC_REMOTE_SRC_CUSTOM_WIDGET_AUTONOMOUSROBOTNOTIFICATION_H_
#define OHM_ROBOTIC_REMOTE_SRC_CUSTOM_WIDGET_AUTONOMOUSROBOTNOTIFICATION_H_

#include <QWidget>
#include <QByteArray>
#include <QTimer>


#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <std_msgs/Int16.h>
#include <std_msgs/String.h>

#include <cv_bridge/cv_bridge.h>

#include "ohm_perception_msgs/GetVictim.h"
#include "ohm_perception_msgs/Victim.h"

namespace Ui {
   class AutonomousRobotNotification;
}

/**
 * @class   VictimNotiificationDialog
 * @author  Christian Pfitzner
 * @date    2015-04-16
 *
 * @brief   Dialog for victim notification
 */
class AutonomousRobotNotification : public QWidget
{
   Q_OBJECT
public:
   /**
    * Function to receive instance of singleton pattern
    * @return
    */
//   static AutonomousRobotNotification* getInstance(void);
   /**
    * private constructor for singleton pattern
    */
   AutonomousRobotNotification(QWidget* parent = 0);

   /**
    * Default destructor
    */
   virtual ~AutonomousRobotNotification(void);

   /**
    * Function to set node handle
    * @param nh
    * @param ns
    */
   void setNodeHandle(ros::NodeHandle* nh, const QByteArray& ns = QByteArray());

   /**
    * Function to be called to update
    */
   void tick(void);

private slots:
   /**
    * Function to send response for victim if rejected or accepted
    */
   void sendVictimResponse(void);

   void showPreview(bool show = true);

   void alarm(void);


private:
   /**
    * Private copy constructor for singleton pattern
    * @param
    */
   AutonomousRobotNotification(const AutonomousRobotNotification& ) { }

//   static AutonomousRobotNotification* _instance;

   void callbackThermalCam(const sensor_msgs::ImageConstPtr& image);
   /**
    * Callback function for image from driving camera
    */
   void callbackDriverCam(const sensor_msgs::ImageConstPtr& image);

   void stateAutonomRobotCallback(const std_msgs::String& state);




   Ui::AutonomousRobotNotification* _ui;

   ros::NodeHandle*                 _nh;                 //!< pointer on node handle
   QByteArray                       _ns;                 //!< namespace for topic

   cv::Mat                          _imgThermal;
   cv::Mat                          _imgRgb;

   bool                             _alarm;
   QTimer                           _timer;


   ros::ServiceClient               _victimService;      //!< service for victim notification
   ros::Publisher                   _pubVictimResponse;  //!< publisher to validate victim
   ros::Subscriber                  _stateSubscriber;

   image_transport::Subscriber      _subThermal;         //!< holds subscriber for thremal image.
   image_transport::Subscriber      _subDriverCam;       //!< subscriber for driver camera

   ohm_perception_msgs::Victim           _victim;             //!< last victim



};

#endif /* OHM_ROBOTIC_REMOTE_SRC_CUSTOM_WIDGET_AUTONOMOUSROBOTNOTIFICATION_H_ */
