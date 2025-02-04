/*
 * Hud.cpp
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#include "hud/Hud.h"
#include "RemoteNode.h"
#include "hud/menu/IMenuItem.h"

#include "menu/MenuItemModeArm.h"
#include "menu/MenuItemModeDrive.h"
#include "menu/MenuItemFlipperAscent.h"
#include "menu/MenuItemFlipperDescent.h"
#include "menu/MenuItemFlipperFlat.h"
#include "menu/MenuItemFlipperUp4.h"
#include "menu/MenuItemArmHorns.h"
#include "menu/MenuItemNodeDriveReverse.h"

#include "ohm_schroedi_mc/FlipperPreset.h"
#include "ohm_schroedi_mc/DriveCtrl.h"
#include "PerspectiveModel.h"

#include <std_srvs/SetBool.h>
#include <std_msgs/Float32.h>

#include "opencv2/opencv.hpp"
#include <zbar.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

Hud::Hud(RemoteNode& parent, ros::NodeHandle& nh):
_parent(parent),
_actionTriggered(false),
_nh(nh),
_it(nh),
_widget(NULL),
_bufImgSensorHead(new std::vector<unsigned char>),
_bufImgGripper(new std::vector<unsigned char>),
_imgSensorHead(new QImage),
_imgGripper(new QImage),
_iconsConst(QVector<QImage>(6)),
_menu(NULL),
_lastRemoteProfile(IMapper::RemoteType::HUD),
_reverseMode(false),
_armActive(false),
_angleGripperHead(0.0)
{
  ros::NodeHandle prvNh("~");

  //  std::string topicFlipperPreset;
  std::string folderPics;
  //std::string topicHeavyMetal;
  std::string topicCamSensorHead;
  std::string topicCamGripper;
  std::string topicMcRet;
  std::string topicGripper1;
  std::string topicGripper2;
  std::string topicAngleGripperHead;
  std::string topicInverseCoordinateSystem;
  std::string topicArmConstraintsActive;


  //  prvNh.param<std::string>("topic_flipper_preset", topicFlipperPreset, "flipper/preset");
  prvNh.param<std::string>("folder_pics", folderPics, "fedd");///home/phil/workspace/ros/src/ohm_teleop/ohm_schr_remote/src/hud/menu_pics");
  //  prvNh.param<std::string>("topic_heavy_metal", topicHeavyMetal, "heavy_metal_mode");
  prvNh.param<std::string>("topic_cam_sensor_head", topicCamSensorHead, "usb_cam/image_raw");
  prvNh.param<std::string>("topic_cam_gripper", topicCamGripper, "usb_cam/image_raw");
  prvNh.param<std::string>("topic_mc_ret", topicMcRet, "mc_ret");
  prvNh.param<std::string>("topic_gripper_1", topicGripper1, "/gripper1/pos/des");
  prvNh.param<std::string>("topic_gripper_2", topicGripper2, "/gripper2/pos/des");
  prvNh.param<std::string>("topic_angle_gripper_head", topicAngleGripperHead, "/joint_7/command");
  prvNh.param<std::string>("topic_inverse_coordinate_system", topicInverseCoordinateSystem, "/inverse/gripper_cs_active");
  prvNh.param<std::string>("topic_arm_constraints_active", topicArmConstraintsActive, "/inverse/arm_reached_space_constraints");


  prvNh.param<double>("switch_drive_time", _switchDriveTime, 1.0);

  _subsCamSensorHead = _it.subscribe(topicCamSensorHead, 1, &Hud::callBackImageSensorHead, this, image_transport::TransportHints("compressed"));
  _subsCamGripper    = _it.subscribe(topicCamGripper, 1, &Hud::callBackImageGripper, this, image_transport::TransportHints("compressed"));
  _subsMcRet = _nh.subscribe(topicMcRet, 1, &Hud::callBackMCret, this);
  _subsAngleGripperSensorHead = _nh.subscribe(topicAngleGripperHead, 1, &Hud::callBackAngleGripperHead, this);
  _subsInverseCoordinateSystem = _nh.subscribe(topicInverseCoordinateSystem, 1, &Hud::callBackInverseCoordinateSystem, this);
  _subsInverseInRestriction = _nh.subscribe(topicArmConstraintsActive, 1, &Hud::callBackInverseArmInRestriction, this);

  _pubImageHud = _it.advertise("hud/hud", 1);
  _pubReImageGripper = _it.advertise("hud/gripper", 1);

  //  _pubGripper1 = _nh.advertise<std_msgs::Float32>(topicGripper1, 1);
  //  _pubGripper2 = _nh.advertise<std_msgs::Float32>(topicGripper2, 1);


  ros::ServiceClient _clientSpreadIt;
  //_subsPu = _nh.subscribe(topicPu, 1, &Hud::callBackPu, this);

  _widget = new HudMainWindow(*this);
  _widget->show();

  QImage image;
  _folderPics = QString(folderPics.c_str());
  if(image.load(_folderPics + "/arm.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_ARM)] = image;
  if(image.load(_folderPics + "/drive.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_DRIVE)] = image;
  if(image.load(_folderPics + "/cross.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::ERROR)] = image;
  if(image.load(_folderPics + "/drive_rev.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_REVERSE)] = image;
  if(image.load(_folderPics + "/gripper.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REFERENCE)] = image;
  if(image.load(_folderPics + "/youshallnotpass.png"))
      _iconsConst[static_cast<unsigned int>(IconsConst::YOU_SHALL_NOT_PASS)] = image;

  _widget->setIconReference(& _iconsConst[static_cast<unsigned int>(IconsConst::REFERENCE)]);
  _widget->setIconShallNotPass(_iconsConst[static_cast<unsigned int>(IconsConst::YOU_SHALL_NOT_PASS)]);

  _timerSwitchDriveMode = ros::Time::now();
}

Hud::~Hud()
{
  // TODO Auto-generated destructor stub
}

void Hud::initMenu(const bool serviceFlipperPresent, const bool serviceHornsPresent)
{
  QString pathError = _folderPics + QString("/cross.png");
  _menu = new MenuItemModeDrive(_folderPics + QString("/drive.png"), *this);
  _menu->setNext(*_menu);
  _menu->setPrev(*_menu);
  //_menu->init();
  _menu->pushBack(*(new MenuItemNodeDriveReverse(_folderPics + QString("/drive_rev.png"), *this)), *_menu);
  _menu->pushBack(*(new MenuItemModeArm(_folderPics + QString("/arm.png"), *this)), *_menu);
  if(!serviceFlipperPresent)
  {
    _menu->pushBack(*(new MenuItemFlipperAscent(pathError, *this)), *_menu);
    _menu->pushBack(*(new MenuItemFlipperUp4   (pathError, *this)), *_menu);
    _menu->pushBack(*(new MenuItemFlipperFlat(pathError, *this)), *_menu);
    //_menu->pushBack(*(new MenuItemFlipperDescent(pathError, *this)), *_menu);
  }
  else
  {
    _menu->pushBack(*(new MenuItemFlipperAscent(_folderPics + QString("/ascent.png"), *this)), *_menu);
    _menu->pushBack(*(new MenuItemFlipperFlat(_folderPics + QString("/flat.png"), *this)), *_menu);
    //    _menu->next()->initUpDown();
    //    _menu->next()->setUp(*_menu->next());
    //      _menu->next()->setDown(*_menu->next());
    //     _menu->next()->pushTop(*(new MenuItemFlipperAscent(_folderPics + QString("/ascent.png"), *this)), *_menu);
    //      _menu->next()->pushTop(*(new MenuItemFlipperUp4   (_folderPics + QString("/up4.png"), *this)), *_menu);

    _menu->pushBack(*(new MenuItemFlipperUp4   (_folderPics + QString("/up4.png"), *this)), *_menu);
  }
  //  if(!serviceHornsPresent)
  //  {
  //    _menu->pushBack(*(new MenuItemArmHorns(pathError, *this)), *_menu);
  //  }
  //  else
  //  {
  //    _menu->pushBack(*(new MenuItemArmHorns(_folderPics + QString("/horns.png"), *this)), *_menu);
  //  }

}

//void Hud::setRemoteOverride(void)
//{
//  _parent.setRemoteOverride(false);
//}

void Hud::buttonPressed(const Buttons_ps3_sixad& button)
{
  if(button == B_LEFT)
  {
    _menu = _menu->previous();
    _widget->setMenuItem(_menu);
  }
  else if(button == B_RIGHT)
  {
    _menu = _menu->next();
    _widget->setMenuItem(_menu);
  }
  else if(button == B_SELECT)
  {
    _widget->setActionTriggered(true);
    _menu->action();
  }
  else if(button == B_PS)
  {
    _parent.switchMapper(_lastRemoteProfile, true);
    _widget->setMenuItem(NULL);
    _widget->setActionTriggered(false);
  }

  //  if(!(joy.buttons[B_L1] && joy.buttons[B_R1]))
  //  {
  //    _timerSwitchDriveMode = ros::Time::now();
  //  }
  //  else
  //  {
  //    const double sec = (ros::Time::now() - _timerSwitchDriveMode).toSec();
  //    std::cout << __PRETTY_FUNCTION__ << " buttons pressed since " << sec << std::endl;
  //    if(sec > _switchTime)
  //
  //  }


  _widget->update();
}
//void Hud::joyCommand(const sensor_msgs::Joy& joy)
//{
//}

void Hud::sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset)
{
  _widget->setActionTriggered(true);
  if(_parent.sendFlipperPreset(preset))
  {
    _widget->setActionTriggered(false);
    _parent.switchMapper(_lastRemoteProfile, true);    //todo: these three calls should be in a method
    _widget->setMenuItem(NULL);
  }
  else
    std::cout << __PRETTY_FUNCTION__ << " flipper service not reachable " << std::endl;
}

void Hud::callBackImageSensorHead(const sensor_msgs::ImageConstPtr& img)
{
  if(img->data.size() != _bufImgSensorHead->size())
    _bufImgSensorHead->resize(img->data.size());
  *_bufImgSensorHead = img->data;
  QImage image(_bufImgSensorHead->data(), img->width, img->height, QImage::Format_RGB888);
  *_imgSensorHead = image;
  //std::vector<QrCode> qrs;
  _widget->setArmActive(_armActive);
  if(!_armActive)
  {
    _widget->setImageSensorHead(*_imgSensorHead);

  }
  else
  {
    _widget->setImageGripper(*_imgSensorHead);
  }
//   _widget->setQrs(qrs);
//  if(_pubImageHud.getNumSubscribers())
//  {
//    QImage imgHud;
//    _widget->paintToHud(&image);
//    sensor_msgs::Image imageRos;
//    imageRos.height = image.height();
//  }
  _widget->update();
}

void Hud::callBackImageGripper(const sensor_msgs::ImageConstPtr& img)
{
  if(img->data.size() != _bufImgGripper->size())
    _bufImgGripper->resize(img->data.size());
  *_bufImgGripper = img->data;
  QImage image(_bufImgGripper->data(), img->width, img->height, QImage::Format_RGB888);
  image = image.rgbSwapped();
  *_imgGripper = image;
  std::vector<QrCode> qrs;
  if(!_armActive)
    _widget->setImageGripper(*_imgGripper);
  else
  {
    _widget->setImageSensorHead(*_imgGripper);



//    std::cout << __PRETTY_FUNCTION__ << " detected  " << qrs.size() << " qrs " << std::endl;
  }
//  if(_pubReImageGripper.getNumSubscribers())
//    _pubReImageGripper(img);
  this->decoceQrs(img, qrs);
  _widget->setQrs(qrs);
  _widget->update();
}

void Hud::callBackImageThermal(const sensor_msgs::ImageConstPtr& img)
{

}

void Hud::menuActive(const IMapper::RemoteType& lastType)
{
  _lastRemoteProfile = lastType;
  _widget->setMenuItem(_menu);
  _widget->update();
  //_menuActive = true;
}

void Hud::changeProfile(const ohm_teleop_msgs::RemoteProfile& profile)
{
  IMapper::RemoteType type;
  switch(profile.remote)
  {
  case ohm_teleop_msgs::RemoteProfile::MAP_DRIVE:
  {
    type = IMapper::RemoteType::DRIVE;
    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_DRIVE)]);
    break;
  }
  case ohm_teleop_msgs::RemoteProfile::MAP_REV:
  {
    type = IMapper::RemoteType::DRIVE_REV;
    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_REVERSE)]);
    break;
  }
  case ohm_teleop_msgs::RemoteProfile::HUD:
  {
    type = IMapper::RemoteType::HUD;
    break;
  }
  case ohm_teleop_msgs::RemoteProfile::MAP_ARM:
  {
    type = IMapper::RemoteType::ARM;
    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_ARM)]);
    break;
  }
  default:
    break;
  }
  if(_parent.switchMapper(type, false))
  {
    _widget->setActionTriggered(false);
    _widget->setMenuItem(NULL);
  }

}

void Hud::referenceDriveTriggered()
{
  //  if(!_parent.sendDriveCommand(ohm_schroedi_mc::DriveCtrl::Request::REFERENCE))
  //    _widget->errorMessage(QString("Reference Drives Failed"));
  bool res = _widget->referenceDriveMenu();
  if(res)
    std::cout << __PRETTY_FUNCTION__ << " hey ho lets go " << std::endl;
  else
    std::cout << __PRETTY_FUNCTION__ << " naaaaaa " << std::endl;
  _parent.sendDriveCommand(ohm_schroedi_mc::DriveCtrl::Request::REFERENCE);
}

void Hud::newPu(const ohm_power_unit::pu& pu)
{
  _widget->setPuVals(pu.r_24V, pu.r_48V);
}

void Hud::setCurrentProfile(const IMapper::RemoteType& type)
{
  switch(type)
  {
  case IMapper::RemoteType::DRIVE:
  {
    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_DRIVE)]);
    break;
  }
  case IMapper::RemoteType::DRIVE_REV:
  {
    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_REVERSE)]);
    break;
  }
  case IMapper::RemoteType::HUD:
  {
    //    _widget->setMenuItem(_menu);
    //    _widget->update();
    break;
  }
  case IMapper::RemoteType::ARM:
  {
    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_ARM)]);
    break;
  }
  default:
    break;
  }
}

void Hud::rdRequired(const bool flag)
{
  if(flag)
    _widget->setIconReference(&_iconsConst[static_cast<unsigned int>(IconsConst::REFERENCE)]);
  else
    _widget->setIconReference(NULL);
}

void Hud::callBackMCret(const ohm_schroedi_mc::mc_ret& mcRet)
{
  //mcRet.error = 32bit uint vorderstes bit R nächstes L
  //mcRet.error & 1 << 32
  std::vector<double> angles(4, 0.0);
  if(!_reverseMode)
  {
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::FRONT_LEFT)]  = -1.0 * (this->flipperTicksToAngle(mcRet.flipper_front_left_ticks_ret)) - M_PI / 2.0;
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::FRONT_RIGHT)] = -1.0 * (this->flipperTicksToAngle(mcRet.flipper_front_right_ticks_ret)) - M_PI / 2.0;
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::REAR_LEFT)]   = this->flipperTicksToAngle(mcRet.flipper_rear_left_ticks_ret) -  M_PI / 2.0;
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::REAR_RIGHT)]  = this->flipperTicksToAngle(mcRet.flipper_rear_right_ticks_ret) - M_PI / 2.0;
  }
  else
  {
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::FRONT_LEFT)]  = -1.0 * (this->flipperTicksToAngle(mcRet.flipper_rear_right_ticks_ret))  + M_PI / 2.0;
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::FRONT_RIGHT)] = -1.0 * (this->flipperTicksToAngle(mcRet.flipper_rear_left_ticks_ret)) + M_PI / 2.0;
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::REAR_LEFT)]   = (this->flipperTicksToAngle(mcRet.flipper_front_right_ticks_ret))  +  M_PI / 2.0;
    angles[static_cast<unsigned int>(PerspectiveModel::Flipper::REAR_RIGHT)]  = (this->flipperTicksToAngle(mcRet.flipper_front_left_ticks_ret)) + M_PI / 2.0;
  }
  _widget->setFlipperAngles(&angles);
  _widget->update();
}

void Hud::callBackAngleGripperHead(const ohm_actors_msgs::DriveState& angle)
{
  // std::cout << __PRETTY_FUNCTION__ << " huhu " << std::endl;
  _widget->setAngleGripperHead(angle.position);
  _widget->update();
}

double Hud::flipperTicksToAngle(const int32_t& ticks)
{
  double var = static_cast<double>(ticks);
  return (var / (GEAR_R * TICS_P_TURN)) * 2.0 * M_PI;
}

void Hud::setHeavyMetalMode(const bool active)
{
  if(_parent.sendHorns(active))
  {
    std::cout << __PRETTY_FUNCTION__ << " heavy metal mode successful " << std::endl;
    _widget->setActionTriggered(false);
    _parent.switchMapper(_lastRemoteProfile, true);    //todo: these three calls should be in a method
    _widget->setMenuItem(NULL);
  }
  else
  {
    std::cout << __PRETTY_FUNCTION__ << " heavy metal mode failed " << std::endl;
    _widget->setActionTriggered(true);
  }
}

const bool Hud::getHeavyMetalMode(void)const
{
  return _parent.flagHorns();
}

void Hud::spreadGripper(void)
{
  std_msgs::Float32 msg;
  msg.data = -1.5;
  _pubGripper1.publish(msg);
  msg.data = 1.5;
  _pubGripper2.publish(msg);
}

void Hud::resetTrackDrives(void)
{
  if(!_parent.sendDriveCommand(ohm_schroedi_mc::DriveCtrl::Request::RESET))
    _widget->errorMessage(QString("Reset Drives Failed"));
}

void Hud::decoceQrs(const sensor_msgs::ImageConstPtr& img, std::vector<QrCode>& qrs)
{
  cv_bridge::CvImagePtr cv_ptr;
  static  zbar::ImageScanner scanner;
  try
  {
    cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
  cv::Mat frame = cv_ptr->image;

  cv::Mat grey;

  cv::cvtColor(frame, grey, CV_BGR2GRAY);

  zbar::Image image(grey.cols, grey.rows, "Y800", grey.data, grey.cols * grey.rows);
  int n = scanner.scan(image);
  for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
  {
    QrCode qr;
    std::vector<cv::Point> corners;
    for (unsigned int i = 0; i < symbol->get_location_size(); i++)
    {
      corners.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
      qr.corners.push_back(QPoint(symbol->get_location_x(i), symbol->get_location_y(i)));
    }
    cv::RotatedRect rotRect = minAreaRect(corners);

    cv::Point qrCenter;
    qrCenter = (corners[0] + corners[2]) * 0.5;
//    unsigned int width = corners[1] - corners[0]) / 2;
//    unsigned int height = (corners[2] + corners[3]) / 2;
    QPoint qCenter(qrCenter.x, qrCenter.y);

    qr.center = qCenter;
    qr.message = symbol->get_data();
    qrs.push_back(qr);
  }
}

void Hud::callBackInverseCoordinateSystem(const std_msgs::Bool& val)
{
  _widget->setActiveCoordinateSystem(val.data);
}

void Hud::callBackInverseArmInRestriction(const std_msgs::Bool& val)
{
    _widget->setArmConstraintsActive(val.data);
}
