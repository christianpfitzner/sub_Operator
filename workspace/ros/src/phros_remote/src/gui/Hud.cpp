/*
 * Hud.cpp
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#include "gui/Hud.h"
//#include "hud/menu/IMenuItem.h"

#include "menu/MenuItemModeArm.h"
#include "menu/MenuItemModeDrive.h"
#include "menu/MenuItemFlipperAscent.h"
#include "menu/MenuItemFlipperDescent.h"
#include "menu/MenuItemFlipperFlat.h"
#include "menu/MenuItemFlipperUp4.h"
#include "menu/MenuItemArmInitialIdle.h"
#include "menu/MenuItemArmInitialLookUp.h"
#include "menu/MenuItemArmInitialLowered.h"
#include "menu/MenuItemArmInitialLookSide.h"
#include "menu/MenuItemArmInitialLookDown.h"
//#include "menu/MenuItemArmHorns.h"
#include "menu/MenuItemModeDriveReverse.h"
#include "menu/MenuItemModeInspect.h"
#include "DialogWarningArmReset.h"
#include "Communication.h"

//#include "PerspectiveModel.h"


namespace phros_remote
{

std::shared_ptr<Hud> Hud::_instance(nullptr);

std::shared_ptr<Hud> Hud::getInstance()
{
  if(!_instance)
    _instance = std::shared_ptr<Hud>(new Hud);
  return _instance;
}

Hud::Hud():
                    _actionTriggered(false),
                    _reverseMode(false),
                    _armActive(false),
                    _iconsConst(7, nullptr),    //Todo: eeeevil magic number
                    _guiUi(std::unique_ptr<Ui::HudMain>(new Ui::HudMain))
{
  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  _guiUi->setupUi(this);
  connect(_guiUi->actionArmReset, SIGNAL(triggered()), this, SLOT(requestArmReset()));
  //  std::cout << __PRETTY_FUNCTION__ << " constructor " << std::endl;
  //  std::cout << __PRETTY_FUNCTION__ << " const items " << std::endl;
  //  for(auto& iter : _iconsConst)
  //    std::cout << __PRETTY_FUNCTION__ << " " << iter << std::endl;
  


  QImage image;
  if(image.load(":/menu_icons/drive.png"))  // + "/arm.png")
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_DRIVE)] = std::shared_ptr<QImage>(new QImage(image));
  else
    std::cout << __PRETTY_FUNCTION__ << "error0" << std::endl;
  if(image.load(":/menu_icons/drive_rev.png"))  // + "/arm.png")
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_REVERSE)] = std::shared_ptr<QImage>(new QImage(image));
  else
      std::cout << __PRETTY_FUNCTION__ << "error1" << std::endl;
  if(image.load(":/menu_icons/arm.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_ARM)] = std::shared_ptr<QImage>(new QImage(image));
  else
      std::cout << __PRETTY_FUNCTION__ << "error2" << std::endl;
  if(image.load(":/menu_icons/cross.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::ERROR)] =  std::shared_ptr<QImage>(new QImage(image));
  else
      std::cout << __PRETTY_FUNCTION__ << "error3" << std::endl;
  if(image.load(":/menu_icons/gripper.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REFERENCE)] = std::shared_ptr<QImage>(new QImage(image));
  else
      std::cout << __PRETTY_FUNCTION__ << "error4" << std::endl;
  if(image.load(":/menu_icons/youshallnotpass.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::YOU_SHALL_NOT_PASS)] =  std::shared_ptr<QImage>(new QImage(image));
  else
      std::cout << __PRETTY_FUNCTION__ << "error5" << std::endl;
      if(image.load(":/menu_icons/inspection.png"))
    _iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_INSPECT)] =  std::shared_ptr<QImage>(new QImage(image));


  this->initMenu(false, false);
  std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
}

Hud::~Hud()
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
}

void Hud::initMenu(const bool serviceFlipperPresent, const bool serviceHornsPresent)
{
  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  try
  {
    /* code */
  
  
  
  _menu = new MenuItemModeDrive(":/menu_icons/drive.png");//   new MenuItemModeDrive(":/menu_icons/drive.png"));
  _menu->init(new MenuItemModeDriveReverse(":/menu_icons/drive_rev.png"));
  _menu->pushBack(new MenuItemModeArm(":/menu_icons/arm.png"), _menu);
  _menu->pushBack(new MenuItemModeInspect(":/menu_icons/inspection.png"), _menu);
  

  IMenuItem* flipperItem = new MenuItemFlipperFlat(":/menu_icons/flat.png");
  _menu->pushBack(flipperItem, _menu);
  flipperItem->initUpDown(new MenuItemFlipperAscent(":/menu_icons/ascent.png"));
  //flipperItem->pushTop(new MenuItemFlipperAscent(":/menu_icons/ascent.png"), flipperItem);
  flipperItem->pushTop(new MenuItemFlipperUp4(":/menu_icons/up4.png"), flipperItem);
  flipperItem->pushTop(new MenuItemFlipperDescent(":/menu_icons/flipper_descend.png"), flipperItem);
  
  // IMenuItem* armInitialItem = new MenuItemArmInitialIdle(":/menu_icons/arm_idle.png");
  // _menu->pushBack(armInitialItem, _menu);
  // armInitialItem->initUpDown(new MenuItemArmInitialLookDown(":/menu_icons/arm_down.png"));
  // armInitialItem->pushTop(new MenuItemArmInitialLowered(":/menu_icons/arm_lowered.png"), armInitialItem);
  // armInitialItem->pushTop(new MenuItemArmInitialLookUp(":/menu_icons/arm_lookup.png"), armInitialItem);
  // armInitialItem->pushTop(new MenuItemArmInitialLookSide(":/menu_icons/arm_look_side.png"), armInitialItem);
  }
  catch(const std::string& e)
  {
    std::cout << __PRETTY_FUNCTION__ << " error : " << e << std::endl;

  }

  //  QString pathError = _folderPics + QString("/cross.png");
  //_menu = std::make_shared<MenuItemModeDrive>(":/menu_icons/drive.png");//   new MenuItemModeDrive(":/menu_icons/drive.png"));
  // std::cout << __PRETTY_FUNCTION__ << "root is " << _menu->type() << std::endl;
  //_menu->init(std::make_shared<MenuItemModeDriveReverse>(":/menu_icons/drive_rev.png"));
  //std::cout << __PRETTY_FUNCTION__ << "root is " << _menu->type() << std::endl;
  // std::cout << __PRETTY_FUNCTION__ << "menu addi " << _menu << std::endl;

  //_menu->
  // _menu->pushBack(std::make_shared<MenuItemModeArm>(":/menu_icons/arm.png"), _menu);
  //    std::cout << __PRETTY_FUNCTION__ << " root counter " << _menu.use_count() << std::endl;
  //    std::cout << __PRETTY_FUNCTION__ << "menu addi " << _menu << std::endl;
  //   _menu->pushBack(*(new MenuItemModeArm(_folderPics + QString("/arm.png"), *this)), *_menu);
  //  if(!serviceFlipperPresent)
  //  {
  //    _menu->pushBack(*(new MenuItemFlipperAscent(pathError, *this)), *_menu);
  //    _menu->pushBack(*(new MenuItemFlipperUp4   (pathError, *this)), *_menu);
  //    _menu->pushBack(*(new MenuItemFlipperFlat(pathError, *this)), *_menu);
  //    //_menu->pushBack(*(new MenuItemFlipperDescent(pathError, *this)), *_menu);
  //  }
  //  else
  //  {
  //    _menu->pushBack(*(new MenuItemFlipperAscent(_folderPics + QString("/ascent.png"), *this)), *_menu);
  //    _menu->pushBack(*(new MenuItemFlipperFlat(_folderPics + QString("/flat.png"), *this)), *_menu);
  //    //    _menu->next()->initUpDown();
  //    //    _menu->next()->setUp(*_menu->next());
  //    //      _menu->next()->setDown(*_menu->next());
  //    //     _menu->next()->pushTop(*(new MenuItemFlipperAscent(_folderPics + QString("/ascent.png"), *this)), *_menu);
  //    //      _menu->next()->pushTop(*(new MenuItemFlipperUp4   (_folderPics + QString("/up4.png"), *this)), *_menu);
  //
  //    _menu->pushBack(*(new MenuItemFlipperUp4   (_folderPics + QString("/up4.png"), *this)), *_menu);
  //  }
  //    if(!serviceHornsPresent)
  //    {
  //      _menu->pushBack(*(new MenuItemArmHorns(pathError, *this)), *_menu);
  //    }
  //    else
  //    {
  //      _menu->pushBack(*(new MenuItemArmHorns(_folderPics + QString("/horns.png"), *this)), *_menu);
  //    }
  std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
}

//void Hud::setRemoteOverride(void)
//{
//  _parent.setRemoteOverride(false);
//}

void Hud::requestArmReset(void)
{
  static DialogWarningArmReset dialogArmReset;
  if(dialogArmReset.exec() == QDialog::Accepted)
  {
    if(!Communication::getInstance()->requestArmReset())
      ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " call to reset arm service failed");
    std::cout << __PRETTY_FUNCTION__ << " yeah go for it " << std::endl; 
  }
  else
  {
    std::cout << __PRETTY_FUNCTION__ << " nope " << std::endl;
  }
    
}

void Hud::buttonPressed(const ButtonsPs3& button)
{
  //  if(button == B_LEFT)
  //  {
  //    _menu = _menu->previous();
  //    _widget->setMenuItem(_menu);
  //  }
  //  else if(button == B_RIGHT)
  //  {
  //    _menu = _menu->next();
  //    _widget->setMenuItem(_menu);
  //  }
  //  else if(button == B_SELECT)
  //  {
  //    _widget->setActionTriggered(true);
  //    _menu->action();
  //  }
  //  else if(button == B_PS)
  //  {
  //    _parent.switchMapper(_lastRemoteProfile, true);
  //    _widget->setMenuItem(NULL);
  //    _widget->setActionTriggered(false);
  //  }
  //
  //  //  if(!(joy.buttons[B_L1] && joy.buttons[B_R1]))
  //  //  {
  //  //    _timerSwitchDriveMode = ros::Time::now();
  //  //  }
  //  //  else
  //  //  {
  //  //    const double sec = (ros::Time::now() - _timerSwitchDriveMode).toSec();
  //  //    std::cout << __PRETTY_FUNCTION__ << " buttons pressed since " << sec << std::endl;
  //  //    if(sec > _switchTime)
  //  //
  //  //  }
  //
  //
  //  _widget->update();
}
//void Hud::joyCommand(const sensor_msgs::Joy& joy)
//{
//}

//void Hud::sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset)
//{
//  _widget->setActionTriggered(true);
//  if(_parent.sendFlipperPreset(preset))
//  {
//    _widget->setActionTriggered(false);
//    _parent.switchMapper(_lastRemoteProfile, true);    //todo: these three calls should be in a method
//    _widget->setMenuItem(NULL);
//  }
//  else
//    std::cout << __PRETTY_FUNCTION__ << " flipper service not reachable " << std::endl;
//}

//void Hud::menuActive(const IMapper::RemoteType& lastType)
//{
//  _lastRemoteProfile = lastType;
//  _widget->setMenuItem(_menu);
//  _widget->update();
//  //_menuActive = true;
//}

//void Hud::changeProfile(const ohm_teleop_msgs::RemoteProfile& profile)
//{
//  IMapper::RemoteType type;
//  switch(profile.remote)
//  {
//  case ohm_teleop_msgs::RemoteProfile::MAP_DRIVE:
//  {
//    type = IMapper::RemoteType::DRIVE;
//    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_DRIVE)]);
//    break;
//  }
//  case ohm_teleop_msgs::RemoteProfile::MAP_REV:
//  {
//    type = IMapper::RemoteType::DRIVE_REV;
//    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_REVERSE)]);
//    break;
//  }
//  case ohm_teleop_msgs::RemoteProfile::HUD:
//  {
//    type = IMapper::RemoteType::HUD;
//    break;
//  }
//  case ohm_teleop_msgs::RemoteProfile::MAP_ARM:
//  {
//    type = IMapper::RemoteType::ARM;
//    _widget->setIconMode(_iconsConst[static_cast<int>(IconsConst::REMOTE_ARM)]);
//    break;
//  }
//  default:
//    break;
//  }
//  if(_parent.switchMapper(type, false))
//  {
//    _widget->setActionTriggered(false);
//    _widget->setMenuItem(NULL);
//  }
//
//}

//void Hud::referenceDriveTriggered()
//{
//  //  if(!_parent.sendDriveCommand(ohm_schroedi_mc::DriveCtrl::Request::REFERENCE))
//  //    _widget->errorMessage(QString("Reference Drives Failed"));
//  bool res = _widget->referenceDriveMenu();
//  if(res)
//    std::cout << __PRETTY_FUNCTION__ << " hey ho lets go " << std::endl;
//  else
//    std::cout << __PRETTY_FUNCTION__ << " naaaaaa " << std::endl;
//  _parent.sendDriveCommand(ohm_schroedi_mc::DriveCtrl::Request::REFERENCE);
//}

//void Hud::newPu(const ohm_power_unit::pu& pu)
//{
//  _widget->setPuVals(pu.r_24V, pu.r_48V);
//}

void Hud::setCurrentProfile(const IMapper::RemoteType& type)
{
  switch(type)
  {
  case IMapper::RemoteType::DRIVE:
  {
    _guiUi->widget->setIconMode(_iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_DRIVE)]);
    break;
  }
  case IMapper::RemoteType::DRIVE_REV:
  {
    _guiUi->widget->setIconMode(_iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_REVERSE)]);
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
    _guiUi->widget->setIconMode(_iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_ARM)]);
    break;
  }
  case IMapper::RemoteType::INSPECT:
  {
    _guiUi->widget->setIconMode(_iconsConst[static_cast<unsigned int>(IconsConst::REMOTE_INSPECT)]);
    break;
  }
  default:
    break;
  }
}

//void Hud::rdRequired(const bool flag)
//{
//  if(flag)
//    _widget->setIconReference(&_iconsConst[static_cast<unsigned int>(IconsConst::REFERENCE)]);
//  else
//    _widget->setIconReference(NULL);
//}

void Hud::setFlipperAngles(const ohm_schroedi_mc::mc_ret& mcRet)
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
  _guiUi->widget->setFlipperAngles(angles);
  this->update();
}


double Hud::flipperTicksToAngle(const int32_t& ticks)
{
  double var = static_cast<double>(ticks);
  return (var / (GEAR_R * TICS_P_TURN)) * 2.0 * M_PI;
}

//void Hud::setHeavyMetalMode(const bool active)
//{
//  if(_parent.sendHorns(active))
//  {
//    std::cout << __PRETTY_FUNCTION__ << " heavy metal mode successful " << std::endl;
//    _widget->setActionTriggered(false);
//    _parent.switchMapper(_lastRemoteProfile, true);    //todo: these three calls should be in a method
//    _widget->setMenuItem(NULL);
//  }
//  else
//  {
//    std::cout << __PRETTY_FUNCTION__ << " heavy metal mode failed " << std::endl;
//    _widget->setActionTriggered(true);
//  }
//}



//const bool Hud::getHeavyMetalMode(void)const
//{
//  return _parent.flagHorns();
//}


//void Hud::resetTrackDrives(void)
//{
////  if(!_parent.sendDriveCommand(ohm_schroedi_mc::DriveCtrl::Request::RESET))
////    _widget->errorMessage(QString("Reset Drives Failed"));
//}

//void Hud::decoceQrs(const sensor_msgs::ImageConstPtr& img, std::vector<QrCode>& qrs)
//{
//  cv_bridge::CvImagePtr cv_ptr;
//  static  zbar::ImageScanner scanner;
//  try
//  {
//    cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
//  }
//  catch (cv_bridge::Exception& e)
//  {
//    ROS_ERROR("cv_bridge exception: %s", e.what());
//    return;
//  }
//  cv::Mat frame = cv_ptr->image;
//
//  cv::Mat grey;
//
//  cv::cvtColor(frame, grey, CV_BGR2GRAY);
//
//  zbar::Image image(grey.cols, grey.rows, "Y800", grey.data, grey.cols * grey.rows);
//  int n = scanner.scan(image);
//  for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
//  {
//    QrCode qr;
//    std::vector<cv::Point> corners;
//    for (unsigned int i = 0; i < symbol->get_location_size(); i++)
//    {
//      corners.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
//      qr.corners.push_back(QPoint(symbol->get_location_x(i), symbol->get_location_y(i)));
//    }
//    cv::RotatedRect rotRect = minAreaRect(corners);
//
//    cv::Point qrCenter;
//    qrCenter = (corners[0] + corners[2]) * 0.5;
////    unsigned int width = corners[1] - corners[0]) / 2;
////    unsigned int height = (corners[2] + corners[3]) / 2;
//    QPoint qCenter(qrCenter.x, qrCenter.y);
//
//    qr.center = qCenter;
//    qr.message = symbol->get_data();
//    qrs.push_back(qr);
//  }
//}

//void Hud::callBackInverseCoordinateSystem(const std_msgs::Bool& val)
//{
//  _widget->setActiveCoordinateSystem(val.data);
//}
//
//void Hud::callBackInverseArmInRestriction(const std_msgs::Bool& val)
//{
//    _widget->setArmConstraintsActive(val.data);
//}


void Hud::showMenu(const bool showHide)
{
  if(showHide)
  {
    _guiUi->widget->setMenu(_menu);
    _guiUi->widget->setActionTriggered(false);
  }
  else
    _guiUi->widget->setMenu(nullptr);
}

void Hud::menuLeft(void)
{
  _menu = &_menu->previous();
  _guiUi->widget->setMenu(_menu);
  this->update();
}

void Hud::menuRight(void)
{
  _menu = &_menu->next();
  _guiUi->widget->setMenu(_menu);
  this->update();
}

void Hud::menuUp(void)
{
  if(!&_menu->up())
    return;
  _menu = &_menu->up();
  _menu->next().setPrev(_menu);
  _menu->previous().setNext(_menu);
  _guiUi->widget->setMenu(_menu);
  this->update();
}

void Hud::menuDown(void)
{
  if(!&_menu->down())
    return;
  _menu = &_menu->down();
  _menu->next().setPrev(_menu);
  _menu->previous().setNext(_menu);
  _guiUi->widget->setMenu(_menu);
  this->update();
}

bool Hud::activateIcon(void)
{
  return _menu->action();
}

}
