/*
 * SensorTestDialog.cpp
 *
 *  Created on: 21.06.2016
 *      Author: chris
 */

#include "SensorTestDialog.h"
#include "ui_SensorTestDialog.h"


#include "ros/master.h"

namespace enc = sensor_msgs::image_encodings;


SensorTestDialog::SensorTestDialog(QWidget* parent, ros::NodeHandle* nh)
:
    QDialog(parent)
,   _ui(new Ui::SensorTestDialog())
,  _nh(nh)
,  _it(*nh)
,  _mode(VIDEO_RESOLUTION)
{
   _ui->setupUi(this);

   connect(_ui->_videoResButton, SIGNAL(released()), this, SLOT(slot_videoresolution()));
   connect(_ui->_motionButton,   SIGNAL(released()), this, SLOT(slot_motion()));
   connect(_ui->_thermalButton,  SIGNAL(released()), this, SLOT(slot_thermal()));
   connect(_ui->_audioButton,    SIGNAL(released()), this, SLOT(slot_audio()));
   connect(_ui->_patternButton,  SIGNAL(released()), this, SLOT(slot_colorpattern()));
   connect(_ui->_gasButton,      SIGNAL(released()), this, SLOT(slot_gas()));


   ros::master::V_TopicInfo topics;
   std::string image_topic = "/image_raw";
   _img_sub        = _it.subscribe(image_topic,        1, &SensorTestDialog::callbackImage, this);
//
   ros::master::getTopics(topics);
   for(const auto& t : topics) {
      std::cout << t.name << " type: " << t.datatype << std::endl;
      if(t.datatype.find("Image") != std::string::npos)
         _ui->_topicCombo->addItem(QString::fromStdString(t.name));
   }

   connect(_ui->_topicCombo,     SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_setImageTopic(QString)));
}

SensorTestDialog::~SensorTestDialog()
{
   delete _ui;
}

void SensorTestDialog::slot_setImageTopic(QString topic)
{
   _img_sub.shutdown();
   _img_sub        = _it.subscribe(topic.toStdString(),        1, &SensorTestDialog::callbackImage, this);
}

void SensorTestDialog::slot_videoresolution(void)
{
   _mode = VIDEO_RESOLUTION;

   this->slot_setImageTopic("/image_raw");
}

void SensorTestDialog::slot_motion(void)
{
   _mode = MOTION;

   this->slot_setImageTopic("/motion_image_topic");
}

void SensorTestDialog::slot_thermal(void)
{
   _mode = THERMAL_IMAGE;
}

void SensorTestDialog::slot_audio(void)
{
   _mode = AUDIO;
}

void SensorTestDialog::slot_colorpattern(void)
{
   _mode = COLOR_PATTERN;
}

void SensorTestDialog::slot_gas(void)
{
   _mode = GAS;
}


void SensorTestDialog::callbackImage(const sensor_msgs::ImageConstPtr& image)
{
   _ui->_imageWidget->setFixedSize(image->width, image->height);

   _ui->_imageWidget->setImage(cv_bridge::toCvShare(image, "rgb8")->image);
}








SensorTestSingletonDialog* SensorTestSingletonDialog::_instance = nullptr;
SensorTestSingletonDialog* SensorTestSingletonDialog::getInstance(void)
{
   if(!_instance) _instance = new SensorTestSingletonDialog();
   return _instance;
}

SensorTestSingletonDialog::~SensorTestSingletonDialog(void)
{
   if(_instance) {
      delete _instance;
      _instance = NULL;
   }
}
//
void SensorTestSingletonDialog::init(void)
{

   // update node handle configuration



}


SensorTestSingletonDialog::SensorTestSingletonDialog(void)
{

}


