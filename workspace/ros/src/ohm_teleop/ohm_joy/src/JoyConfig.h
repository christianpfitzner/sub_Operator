/*
 * JoyConfig.h
 *
 *  Created on: 11.02.2015
 *      Author: chris
 */

#ifndef OHM_JOY_SRC_JOYCONFIG_H_
#define OHM_JOY_SRC_JOYCONFIG_H_

// qt include
#include <QMainWindow>
#include <QTimer>

// ros includes
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>

#include <vector>

#include "JoyConfigInputWidget.h"

/**
 * @namespace Ui
 */
namespace Ui {
class JoyConfig;
}

/**
 * @class   JoyConfig
 * @author  Christian Pfitzner
 * @date    2015-02-11
 */
class JoyConfig : public QMainWindow
{
   Q_OBJECT
public:
   /**
    * Default constructor
    * @param parent
    */
   JoyConfig(QWidget* parent = 0);
   /**
    * Default destructor
    */
   virtual ~JoyConfig(void);

   /**
    * Function to set node handle
    * @param nh
    */
   void setNodeHandle(ros::NodeHandle nh) { _nh = nh; }

private slots:
   void slot_update(unsigned int idx);
   /**
    * Timing slot
    */
   void slot_tick(void);

   void slot_load(void);

   void slot_save(void);

private:
   void joyCallback(const sensor_msgs::Joy& msg);

   Ui::JoyConfig* _ui;

   std::vector<JoyConfigInputWidget*> _axis;
   std::vector<JoyConfigInputWidget*> _buttons;

   JoyConfigInputWidget*              _active_widget;
   bool                               _waiting_for_input;
   QTimer                             _timer;


   // ros members
   ros::NodeHandle _nh;
   ros::Subscriber _joy_sub;
};

#endif /* OHM_JOY_SRC_JOYCONFIG_H_ */
