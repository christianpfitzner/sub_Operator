/*
 * RemoteNode.cpp
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#include "RemoteNode.h"
#include "mappers/MapperDriveReverse.h"
#include "mappers/MapperHud.h"
#include "ohm_actors_msgs/SetHomingSensHead.h"
#include "ohm_schroedi_mc/mc_ctrl.h"
#include <cmath>
#include <QApplication>
#include <std_srvs/Empty.h>

RemoteNode::RemoteNode():
_homingYawFor  (0.0),
_homingPitchFor(0.0),
_homingYawRev  (0.0),
_homingPitchRev(0.0),
_timerMain(new QTimer(this)), //todo: unique_ptr
_hud(*this, _nh),
_mappers(4, NULL),
_flagES(false),
_flagUR(true),
_timerLastPu(ros::Time::now()),
_flagHorns(true)
{
  ros::NodeHandle prvNh("~");

  std::string topicJoy;
  std::string topicSwitchRemote;
  std::string topicActiveRemote;
  std::string topicFlipperPreset;
  std::string topicSetHoming;
  std::string topicReferenceDrive;
  std::string topicPu;
  std::string topicHorns;
  std::string topicResetTrackDrives;
  std::string topicDriveCtrl;
  std::string topicQr;
  double loopRate = 0.0;

  prvNh.param<std::string>("topic_joy", topicJoy, "joy");
  prvNh.param<std::string>("topic_switch_remote", topicSwitchRemote, "switch_remote");
  prvNh.param<std::string>("topic_active_profile", topicActiveRemote, "active_profile");
  prvNh.param<std::string>("topic_flipper_preset", topicFlipperPreset, "flipper/preset");

  prvNh.param<std::string>("topic_set_homing", topicSetHoming, "set_homing");
  prvNh.param<std::string>("topic_reference_drive", topicReferenceDrive, "cmd/mc_ctrl");
  prvNh.param<std::string>("topic_pu", topicPu, "pu");
  prvNh.param<std::string>("topic_horns", topicHorns, "heavy_metal_mode");
  prvNh.param<std::string>("topic_reset_track_drives", topicResetTrackDrives, "reset_drives");
  prvNh.param<std::string>("topic_drive_ctrl", topicDriveCtrl, "drive_control");
  prvNh.param<double>("homing_yaw_for"   , _homingYawFor  , 0.517);
  prvNh.param<double>("homing_pitch_for" , _homingPitchFor, -0.18);
  prvNh.param<double>("homing_yaw_rev"   , _homingYawRev  , -3.11091303825);
  prvNh.param<double>("homing_pitch_rev" , _homingPitchRev, -0.0368155390024);
  prvNh.param<double>("thresh_es", _threshES, 12.0);
  prvNh.param<double>("thresh_switch_dir", _threshSwitchDir, 0.25);
  prvNh.param<std::string>("topic_qr", topicQr, "/qr/pose");

  prvNh.param<double>("loop_rate", loopRate, 100.0);

  _timerMain->start(static_cast<int>(round((1.0 / loopRate) * 1000.0)));

  connect(_timerMain, SIGNAL(timeout()), this, SLOT(callBackTimerMain()));

  _subsJoy = _nh.subscribe(topicJoy, 1, &RemoteNode::callBackJoy, this);
  _subsPu = _nh.subscribe(topicPu, 1, &RemoteNode::callBackPu, this);
  _subsQr = _nh.subscribe(topicQr, 1, &RemoteNode::callBackQr, this);

  _pubActiveProfile = _nh.advertise<ohm_teleop_msgs::RemoteProfile>(topicActiveRemote, 1);
  _pubReferenceCommand = _nh.advertise<ohm_schroedi_mc::mc_ctrl>(topicReferenceDrive, 1);
 // _clientResetTrackDrives = _nh.serviceClient<std_srvs::Empty>(topicResetTrackDrives);
  _clientDriveCtrl = _nh.serviceClient<ohm_schroedi_mc::DriveCtrl>(topicDriveCtrl, 1);

    if(ros::service::waitForService(topicSetHoming, ros::Duration(0.5)))
      _srvSetSensorHoming = _nh.serviceClient<ohm_actors_msgs::SetHomingSensHead>(topicSetHoming);
    else
      ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " warning. Service " << topicSetHoming << " not reachable");

    std::cout << __PRETTY_FUNCTION__ << " waiting for service  " << topicFlipperPreset << std::endl;
    bool flipperPresetActive = false;
    flipperPresetActive = ros::service::waitForService(topicFlipperPreset, ros::Duration(0.5));
    if(flipperPresetActive)
      _clientFlipperPreset = _nh.serviceClient<ohm_schroedi_mc::FlipperPreset>(topicFlipperPreset);
    else
          ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " warning. Service " << topicFlipperPreset << " not reachable");

    bool hornsActive = false;
    std::cout << __PRETTY_FUNCTION__ << " waiting for service  " << topicHorns << std::endl;
    hornsActive = ros::service::waitForService(topicHorns, ros::Duration(0.5));
    if(hornsActive)
      _clientArmHorns = _nh.serviceClient<std_srvs::SetBool>(topicHorns);
    else
          ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " warning. Service " << topicHorns << " not reachable");

    _hud.initMenu(flipperPresetActive, hornsActive);

  _mappers[static_cast<unsigned int>(IMapper::RemoteType::ARM)] = new MapperArm(_nh);
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)] = new MapperDrive(_nh);
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE_REV)] = new MapperDriveReverse(_nh);
  _mappers[static_cast<unsigned int>(IMapper::RemoteType::HUD)] = new MapperHud(_hud);
  const IMapper::RemoteType typeDrive = IMapper::RemoteType::DRIVE;
  _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)];
  _hud.setCurrentProfile(_mapper->type());
  this->setHoming(_homingPitchFor, _homingYawFor);
}

RemoteNode::~RemoteNode()
{
  // TODO Auto-generated destructor stub
}

void RemoteNode::callBackJoy(const sensor_msgs::Joy& joy)
{
  static bool initialized = false;
  static bool psLast = true;
  static bool menuOverride = false;
  static ros::Time timerSwitch = ros::Time::now();
  //  static IMapper::type()
  //map twist
#ifdef OLD
  if((joy.axes[R2] < -0.7 && joy.axes[L2] < -0.7) && !initialized)  //apparently the ps3 pad needs to be initialized...both accelerators pressed
#else
    if((joy.axes[R2] > 0.7 && joy.axes[L2] > 0.7) && !initialized)  //apparently the ps3 pad needs to be initialized...both accelerators pressed
#endif
    {
      ROS_INFO("Joy Node: Initializing...");
      usleep(1000 * 1000);
      initialized = true;
      ROS_INFO("Joy Node: Initialized");
    }
  if(!initialized)
  {
    ROS_INFO_THROTTLE(1.0, "Joy Node: Not initialized");
    return;
  }
  if(joy.buttons[B_PS] && !psLast)
  {
    if(_mapper->type() != IMapper::RemoteType::HUD)
    {
      std::cout << __PRETTY_FUNCTION__ << " try to switch to hud" << std::endl;
      _hud.menuActive(_mapper->type());
      this->switchMapper(IMapper::RemoteType::HUD, false);
      dynamic_cast<MapperHud*>(_mapper)->reset();
    }
  }
  static bool switched = false;
  if(joy.buttons[B_L1] && joy.buttons[B_R1])
  {
    const double timePressed = (ros::Time::now() - timerSwitch).toSec();
   // std::cout << __PRETTY_FUNCTION__ << " buttons pressed since " << timePressed << std::endl;
    if((timePressed > _threshSwitchDir) && !switched)
    {
      if(_mapper->type() == IMapper::RemoteType::DRIVE)
        this->switchMapper(IMapper::RemoteType::DRIVE_REV, false);
      else if(_mapper->type() == IMapper::RemoteType::DRIVE_REV)
        this->switchMapper(IMapper::RemoteType::DRIVE, false);
      timerSwitch = ros::Time::now();
      switched = true;
    }
  }
  else
  {
    timerSwitch = ros::Time::now();
    switched = false;
  }
  _mapper->map(joy);
  psLast = joy.buttons[B_PS];
}

void RemoteNode::callBackTimerMain()
{
  if(!ros::ok())
    QApplication::quit();
  ros::spinOnce();
  ohm_teleop_msgs::RemoteProfile profile;
  if(_mapper->type() == IMapper::RemoteType::DRIVE)
    profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_DRIVE;
  else if(_mapper->type() == IMapper::RemoteType::ARM)
    profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_ARM;
  else if(_mapper->type() == IMapper::RemoteType::DRIVE_REV)
    profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_REV;
  else if(_mapper->type() == IMapper::RemoteType::HUD)
    profile.remote = ohm_teleop_msgs::RemoteProfile::HUD;
  else
    ROS_ERROR("%s error. Unknown profile (should not happen)", __PRETTY_FUNCTION__);
  _pubActiveProfile.publish(profile);
  this->checkCallBacks();
  _hud.esActive(_flagES);
  _hud.rdRequired(_flagUR);
  if(0)//(ros::Time::now() - _qrTimer).toSec() > 1.0)
  {
//    _qrs.qr.clear();
//    _hud.setQrs(_qrs);
  }
}

bool RemoteNode::switchMapper(const IMapper::RemoteType& type, const bool restore)  //todo: do we need this damn bool?
{
  //static IMapper::RemoteType last = _mapper->type();
//  std::cout << __PRETTY_FUNCTION__ << " " << static_cast<unsigned int>(_mapper->type()) << " != "
//            << static_cast<unsigned int>(type) << std::endl;
  if(!restore)//_mapper->type() == IMapper::RemoteType::HUD)
    if(_hud.lastRemoteProfile() == type)
      return false;
  if(_mapper->type() == type)
    return false;
  switch(type)
  {
  case IMapper::RemoteType::DRIVE:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to drive" << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE)];
    dynamic_cast<MapperDrive*>(_mapper)->homing(50);
    this->setHoming(_homingPitchFor, _homingYawFor);
    _hud.setReverseMode(false);
    _hud.setArmActive(false);
    break;
  }
  case IMapper::RemoteType::DRIVE_REV:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to drive reverse " << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::DRIVE_REV)];
    this->setHoming(_homingPitchRev, _homingYawRev);
    dynamic_cast<MapperDriveReverse*>(_mapper)->homing(50);
    _hud.setReverseMode(true);
    _hud.setArmActive(false);
    break;
  }
  case IMapper::RemoteType::ARM:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to arm " << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::ARM)];
    _hud.setArmActive(true);
    break;
  }
  case IMapper::RemoteType::HUD:
  {
    std::cout << __PRETTY_FUNCTION__ << " switch to hud" << std::endl;
    _mapper = _mappers[static_cast<unsigned int>(IMapper::RemoteType::HUD)];
    break;
  }
  default:
  {
    std::cout << __PRETTY_FUNCTION__ << " Error. Unknown profile selected " << std::endl;
    return false;
  }
  }
  _hud.setCurrentProfile(type);
  return true;
}

//void RemoteNode::referenceDrive(void)
//{
//  std::cout << __PRETTY_FUNCTION__ << " reference drive triggered " << std::endl;
//  ohm_schroedi_mc::mc_ctrl mcCtrl;
//  mcCtrl.control = 1;
//  _pubReferenceCommand.publish(mcCtrl);
//  _flagUR = false;
//}

void RemoteNode::callBackPu(const ohm_power_unit::pu& pu)
{
  _timerLastPu = ros::Time::now();
  if(pu.r_48V < _threshES)
  {
    _flagES = true;
    _flagUR = true;
  }
  else
    _flagES = false;
  _hud.newPu(pu);
}

void RemoteNode::checkCallBacks(void)
{
  if((ros::Time::now() -_timerLastPu).toSec() > ::TIMEOUT_CALLBACKS)
  {
    ohm_power_unit::pu pu;
    pu.r_24V = NAN;
    pu.r_48V = NAN;
    _hud.newPu(pu);
  }
}

bool RemoteNode::sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset)
{
  ohm_schroedi_mc::FlipperPreset pre;
  pre.request = preset.request;
  return _clientFlipperPreset.call(pre);
}

bool RemoteNode::sendHorns(const bool active)
{
  if(active == _flagHorns)
    return false;
  std_srvs::SetBool setHorns;
  setHorns.request.data = active;
  if(_clientArmHorns.call(setHorns))
  {
    _flagHorns = active;
    return true;
  }
  else
  {
    return false;
  }
}

bool RemoteNode::setHoming(const double homingPitch, const double homingYaw)
{
  ohm_actors_msgs::SetHomingSensHead setHomin;
  setHomin.request.homingPitch = homingPitch;
  setHomin.request.homingYaw = homingYaw;
  return _srvSetSensorHoming.call(setHomin);
}

//bool RemoteNode::resetTrackDrives(void)
//{
//  std::cout << __PRETTY_FUNCTION__ << " yea " << std::endl;
//  std_srvs::Empty resetDrives;
//  return _clientResetTrackDrives.call(resetDrives);
//}

bool RemoteNode::sendDriveCommand(const ohm_schroedi_mc::DriveCtrl::Request::_command_type cmd)
{
  ohm_schroedi_mc::DriveCtrl ctrl;
  ctrl.request.command = cmd;
  std::cout << __PRETTY_FUNCTION__ << " triggered cmd << " << static_cast<unsigned int>(cmd) << std::endl;
  return _clientDriveCtrl.call(ctrl);
}

void RemoteNode::callBackQr(const ohm_perception_msgs::QrArray& qrs)
{
//  _qrTimer = ros::Time::now();
//  _qrs = qrs;
//  _hud.setQrs(_qrs);
}
