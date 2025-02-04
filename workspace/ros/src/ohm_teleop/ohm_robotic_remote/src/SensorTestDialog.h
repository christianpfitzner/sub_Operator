/*
 * SensorTestDialog.h
 *
 *  Created on: 21.06.2016
 *      Author: chris
 */

#ifndef OHM_TELEOP_OHM_ROBOTIC_REMOTE_SRC_SENSORTESTDIALOG_H_
#define OHM_TELEOP_OHM_ROBOTIC_REMOTE_SRC_SENSORTESTDIALOG_H_

#include <QDialog>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>


#include "ImageWidget.h"


#include <image_transport/image_transport.h>


/**
 * @namespace Ui
 */
namespace Ui {
class SensorTestDialog;
}

/**
 *  @class  SensorTestDialog
 *  @author Christian Pfitzner
 *
 */
class SensorTestDialog : public QDialog
{
   Q_OBJECT
public:
   enum SensorTestEnum {
      VIDEO_RESOLUTION,
      MOTION,
      THERMAL_IMAGE,
      AUDIO,
      COLOR_PATTERN,
      GAS
   };

   /**
    * Default constructor
    */
   SensorTestDialog(QWidget* parent = nullptr, ros::NodeHandle* nh = nullptr);
   /**
    * Default destructor
    */
   virtual ~SensorTestDialog();


   // GETTERS
   /**
    * function to return current mode
    * @return
    */
   SensorTestEnum getMode(void) { return  _mode; }


   // SETTERS
   /**
    * Function to set mode for this widget
    */
   void setMode(SensorTestEnum mode)       { _mode = mode; }
   /**
    * Function to set node handle
    * @param nh
    */
   void setNodeHandle(ros::NodeHandle* nh) { _nh = nh; }


private slots:
   /**
    * Function to set image topic
    * @param topic
    */
   void slot_setImageTopic(QString topic);
   /**
    * Function to set topic for video resolution test
    */
   void slot_videoresolution(void);
   /**
    * Function to set topic for motion check
    */
   void slot_motion(void);
   /**
    * Function to set topic for thermal check
    */
   void slot_thermal(void);
   /**
    * Function to set topic for audio check
    */
   void slot_audio(void);
   /**
    * Function to set topic for color pattern check
    */
   void slot_colorpattern(void);
   /**
    * Function to set topic for gas checks
    */
   void slot_gas(void);


protected:
   /**
    * Image callback function
    * @param image
    */
   void callbackImage(const sensor_msgs::ImageConstPtr& image);

   Ui::SensorTestDialog*            _ui;           //!< pointer for ui

   ros::NodeHandle*                 _nh;           //!< pointer on node handle
   image_transport::ImageTransport  _it;           //!< image tansport

   SensorTestEnum                   _mode;         //!< current mode of this dialog

   image_transport::Subscriber      _img_sub;      //!< subscriber for image topic

};


class SensorTestSingletonDialog : public SensorTestDialog
{
public:
   /**
    * Function to return instance of this singleton
    * @return
    */
   static SensorTestSingletonDialog* getInstance(void);
   /**
    * Default destructor
    */
   virtual ~SensorTestSingletonDialog(void);

   void setNodeHandle(ros::NodeHandle* nh) { _nh = nh; }

   void init(void);

private:
   SensorTestSingletonDialog(void);
//   SensorTestSingletonDialog(SensorTestDialog& ) {}// = delete;

   static SensorTestSingletonDialog* _instance;
};

#endif /* OHM_TELEOP_OHM_ROBOTIC_REMOTE_SRC_SENSORTESTDIALOG_H_ */
