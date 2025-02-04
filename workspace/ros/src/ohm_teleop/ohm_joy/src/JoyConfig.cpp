/*
 * JoyConfig.cpp
 *
 *  Created on: 11.02.2015
 *      Author: chris
 */

#include "JoyConfig.h"

#include <QDebug>
#include <QFileDialog>
#include <qsettings.h>
#include <QStringList>

#include "ui_JoyConfig.h"


JoyConfig::JoyConfig(QWidget* parent)
:
   QMainWindow(parent),
   _ui(new Ui::JoyConfig),
   _waiting_for_input(false)
{
   _ui->setupUi(this);

   for(unsigned int i=0 ; i<10 ; i++) {
      _axis.push_back(new JoyConfigInputWidget(this));
      _axis[i]->setIndex(i);
      _axis[i]->setType(JoyConfigInputWidget::Axis);
      _ui->verticalLayout_Left->addWidget(_axis[i]);
   }

   // basic driving
   _axis[0]->setText("Forward");
   _axis[1]->setText("Backwards");
   _axis[2]->setText("Left-Right");
   _axis[3]->setText("Camera-Up-Down");
   _axis[4]->setText("Camera-Left-Right");

   // settings for manipulator
   _axis[5]->setText("Manipulator: TCP Up-Down");
   _axis[6]->setText("Manipulator: TCP Forward");
   _axis[7]->setText("Manipulator: TCP Backward");
   _axis[8]->setText("Manipulator: Orientation Left-Right");
   _axis[9]->setText("Manipulator: Orientation Up-Down");


   for(unsigned int i=0 ; i<15 ; i++) {
      _buttons.push_back(new JoyConfigInputWidget(this));
      _buttons[i]->setIndex(i+_axis.size());
      _buttons[i]->setType(JoyConfigInputWidget::Button);
      _ui->verticalLayout_Right->addWidget(_buttons[i]);
   }

   // basic action handling
   _buttons[0]->setText("Acknowledged");
   _buttons[1]->setText("Decline");
   _buttons[2]->setText("Switch-Autonomy-Teleop");
   _buttons[3]->setText("Reset-Camera");
   _buttons[4]->setText("Call-Menu");

   _buttons[5]->setText("Change-Namespace");
   _buttons[6]->setText("Change-Viewer");

   // buttons for manipulator
   _buttons[7]->setText("Manipulator: Move Out");
   _buttons[8]->setText("Manipulator: Move Home");
   _buttons[9]->setText("Manipulator: Drive Slowly");

   _buttons[10]->setText("Flipper: Select Front");
   _buttons[11]->setText("Flipper: Select Back");
   _buttons[12]->setText("Flipper: Select Both");

   _buttons[13]->setText("Flipper: up");
   _buttons[14]->setText("Flipper: down");

   // connect all checkboxes to the same slot
   foreach(JoyConfigInputWidget* input, this->findChildren<JoyConfigInputWidget*>()) {
       connect(input, SIGNAL(clicked(unsigned int)), this, SLOT(slot_update(unsigned int)));
   }

   connect(&_timer,           SIGNAL(timeout()),   this, SLOT(slot_tick()));

   connect(_ui->pushButton_load,  SIGNAL(released()),   this, SLOT(slot_load()));
   connect(_ui->pushButton_save,  SIGNAL(released()),   this, SLOT(slot_save()));




   ros::NodeHandle private_nh("~");
   std::string joy_topic;
   private_nh.param("joy_topic",            joy_topic,              std::string("joy"));
   _joy_sub = _nh.subscribe(joy_topic, 20, &JoyConfig::joyCallback, this);

   _timer.start(10);


}

JoyConfig::~JoyConfig()
{
   // clean up
   for(unsigned int i=0 ; i<_axis.size()    ; i++) delete _axis[i];
   for(unsigned int i=0 ; i<_buttons.size() ; i++) delete _buttons[i];
}


void JoyConfig::slot_update(unsigned int idx)
{
   if(_waiting_for_input) {
      _active_widget->setInactive();
      _waiting_for_input = false;
   }

   // iterate over axis
   for(unsigned int i=0 ; i<_axis.size() ; i++)
      {
      if(_axis[i]->getIndex() == idx)
      {
         _active_widget = _axis[i];
         _axis[i]->setActive();
         _waiting_for_input = true;
      }
   }

   // iterate over buttons
   for(unsigned int i=0 ; i<_buttons.size() ; i++)
   {
      if(_buttons[i]->getIndex() == idx)
      {
         _active_widget = _buttons[i];
         _buttons[i]->setActive();
         _waiting_for_input = true;
      }
   }
}

void JoyConfig::slot_tick(void)
{
   ros::spinOnce();
}

void JoyConfig::slot_load(void)
{
   QString path = QFileDialog::getOpenFileName(this, tr("Open File"),
                                               "",
                                               tr("ini (*.ini*)"));

   QSettings settings(path, QSettings::NativeFormat);

   settings.beginGroup("axis");
      // basic driving
      for(unsigned int i=0 ; i<_axis.size() ; i++)
      {
         settings.beginGroup(_axis[i]->getName());
             _axis[i]->setAxisOrButton(settings.value("value").toInt());
          settings.endGroup();
      }
   settings.endGroup();
   settings.beginGroup("buttons");
      for(unsigned int i=0 ; i<_buttons.size() ; i++)
      {
         settings.beginGroup(_buttons[i]->getName());
             _buttons[i]->setAxisOrButton(settings.value("value").toInt());
         settings.endGroup();
      }
   settings.endGroup();
}

void JoyConfig::slot_save(void)
{
   QString path = QFileDialog::getSaveFileName(this, tr("Save File"),
                                               "",
                                               tr("ini (*.ini*)"));

   QSettings settings(path, QSettings::NativeFormat);

   /*
    * save all button setting
    */
   settings.beginGroup("buttons");
   for(unsigned int i=0 ; i<_buttons.size() ; i++)
   {
      settings.beginGroup(_buttons[i]->getName());
         settings.setValue("value", _buttons[i]->getAxisOrButton());
      settings.endGroup();
   }
   settings.endGroup();

   /*
    * save all axis setting
    */
   settings.beginGroup("axis");
   for(unsigned int i=0 ; i<_axis.size() ; i++)
   {
      settings.beginGroup(_axis[i]->getName());
      settings.setValue("value",  _axis[i]->getAxisOrButton());
         settings.setValue("inverted", false);
      settings.endGroup();
   }
   settings.endGroup();


}


void JoyConfig::joyCallback(const sensor_msgs::Joy& msg)
{
   if(!_waiting_for_input)   return;

   int idx = -1;


//   unsigned int axis_size=16;
//   if(msg.axes.size() <16)
//      axis_size = msg.axes.size();

   // check for type to watch in joy
   if(_active_widget->getType() == JoyConfigInputWidget::Button) {
      for(unsigned int i=0 ; i<msg.buttons.size() ; i++) {
         if(std::fabs(msg.buttons[i]) == 1.0) idx = i;
      }
   }
   else {
      for(unsigned int i=0 ; i<msg.axes.size() ; i++) {
         if(std::fabs(  msg.axes[i]) < 0.9 && std::fabs(  msg.axes[i])> 0.5) idx = i;
      }
   }

   if(idx != -1)
   {
      _active_widget->setAxisOrButton(idx);
      _active_widget->setInactive();

      _waiting_for_input = false;
   }
}

