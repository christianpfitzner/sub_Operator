/*
 * VictimNotificationDialog.cpp
 *
 *  Created on: 16.04.2015
 *      Author: chris
 */
#include "ui_AutonomousRobotNotification.h"
#include "AutonomousRobotNotification.h"




//
//AutonomousRobotNotification* AutonomousRobotNotification::_instance = 0;
//
//AutonomousRobotNotification* AutonomousRobotNotification::getInstance(void)
//{
//   if(!_instance) _instance = new AutonomousRobotNotification();
//   return _instance;
//}

AutonomousRobotNotification::AutonomousRobotNotification(QWidget* parent) :
QWidget(parent)
, _ui(new Ui::AutonomousRobotNotification)
, _nh(0)
, _ns("")
, _alarm(false)
{
   _ui->setupUi(this);

   _ui->_rgbView->hide();
   _ui->_thermalView->hide();

   // hide buttons
   _ui->_buttonAccept->hide();
   _ui->_buttonReject->hide();
   _ui->_buttonAccept->setDisabled(true);
   _ui->_buttonReject->setDisabled(true);

   this->connect(_ui->_buttonAccept, SIGNAL(clicked()), this, SLOT(sendVictimResponse()));
   this->connect(_ui->_buttonReject, SIGNAL(clicked()), this, SLOT(sendVictimResponse()));
   this->connect(_ui->_showButton,   SIGNAL(toggled(bool)), this, SLOT(showPreview(bool)));



   this->connect(&_timer, SIGNAL(timeout()), this, SLOT(alarm()));
   _timer.start(150);

}

AutonomousRobotNotification::~AutonomousRobotNotification(void)
{
//   if(_instance) {
//      delete _instance;
//      _instance = 0;
//   }
}


void AutonomousRobotNotification::setNodeHandle(ros::NodeHandle* nh, const QByteArray& ns)
{
   _nh = nh;         // set node handle
   _ns = ns;         // set name space

   _victimService     = _nh->serviceClient<ohm_perception_msgs::GetVictim>("/victim/get_victim");
   _pubVictimResponse = _nh->advertise<ohm_perception_msgs::Victim>("/victim/response", 10);

   _stateSubscriber   = _nh->subscribe("/georg/state", 1, &AutonomousRobotNotification::stateAutonomRobotCallback, this);
}


void AutonomousRobotNotification::tick(void)
{
   ohm_perception_msgs::GetVictim service;
   service.request.id = ohm_perception_msgs::GetVictim::Request::LAST;


   if (_victimService.call(service))
   {
      _ui->_buttonAccept->show();
      _ui->_buttonReject->show();

      _ui->_buttonAccept->setDisabled(false);
      _ui->_buttonReject->setDisabled(false);

      _victim = service.response.victim;

      _alarm = true;

//      image_transport::ImageTransport it(*_nh);
//      _subDriverCam = it.subscribe(  "/georg/image/driver",
//                                     2,
//                                     &AutonomousRobotNotification::callbackDriverCam,
//                                     this);

   }

   // check service servers
   ros::spinOnce();
}

void AutonomousRobotNotification::sendVictimResponse(void)
{
   QPushButton* sender = qobject_cast<QPushButton*>(this->sender());

   if      (sender == _ui->_buttonAccept) _victim.valid = true;
   else if (sender == _ui->_buttonReject) _victim.valid = false;
   else    return;

   _victim.checked = true;
   _pubVictimResponse.publish(_victim);


   // hide viewers
   _ui->_rgbView->hide();
   _ui->_thermalView->hide();

   // hide buttons
   _ui->_buttonAccept->setDisabled(true);
   _ui->_buttonReject->setDisabled(true);

   _alarm = false;

}

void AutonomousRobotNotification::showPreview(bool show)
{
   if(show) {
      _ui->_rgbView->show();
      _ui->_thermalView->show();
      _ui->_buttonAccept->show();
      _ui->_buttonReject->show();
   }
   else {
      _ui->_rgbView->hide();
      _ui->_thermalView->hide();
      _ui->_buttonAccept->hide();
      _ui->_buttonReject->hide();
   }
}

void AutonomousRobotNotification::alarm(void)
{
   static bool old_status;
   bool status = !old_status;

   if(_alarm)
   {
      _ui->_showButton->setText("!!!!!!!!!!!!!!! Victim Found !!!!!!!!!!!!!!!");


      if(status){
         _ui->_showButton->setPalette(QPalette(Qt::black));
         _ui->_showButton->setStyleSheet("background-color: rgb(242, 203, 64); color: rgb(255, 255, 255)");
         _ui->_showButton->setAutoFillBackground(true);
      }
      else {
         _ui->_showButton->setPalette(QPalette(Qt::red));
         _ui->_showButton->setStyleSheet("background-color: rgb(255, 153, 102); color: rgb(255, 255, 255)");
         _ui->_showButton->setAutoFillBackground(true);
      }
   }
   else
   {
      _ui->_showButton->setPalette(QPalette(Qt::black));
      _ui->_showButton->setStyleSheet("background-color: rgb(242, 203, 64); color: rgb(255, 255, 255)");
      _ui->_showButton->setAutoFillBackground(true);
      _ui->_showButton->setText("+");

   }

   old_status = status;
}

void AutonomousRobotNotification::callbackThermalCam(const sensor_msgs::ImageConstPtr& image)
{

   _subThermal.shutdown();
}


void AutonomousRobotNotification::callbackDriverCam(const sensor_msgs::ImageConstPtr& image)
{
   _imgRgb = cv_bridge::toCvShare(image, "rgb8")->image;
   _ui->_rgbView->setImage(_imgRgb);

   _subDriverCam.shutdown();
}

void AutonomousRobotNotification::stateAutonomRobotCallback(const std_msgs::String& state)
{
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   QString text(state.data.c_str());
   _ui->_status->setText(text);
}
