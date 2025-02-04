/*
 * RemoteNode.h
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_REMOTENODE_H_
#define OHM_SCHR_REMOTE_SRC_REMOTENODE_H_

#include <QObject>
#include <QTimer>

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Bool.h>
#include <std_srvs/SetBool.h>
#include "ohm_teleop_msgs/RemoteProfile.h"
#include "ohm_teleop_msgs/SetRemoteProfile.h"
#include "ohm_schroedi_mc/DriveCtrl.h"
#include "mappers/IMapper.h"
#include "mappers/MapperDrive.h"
#include "mappers/MapperArm.h"
#include "mappers/Ps3Profiles.h"
#include "hud/Hud.h"
#include "ohm_power_unit/pu.h"
#include "ohm_perception_msgs/QrArray.h"
#endif

#include <memory>
#include <string>

namespace
{
  const double TIMEOUT_CALLBACKS = 0.5;
}

class RemoteNode : public QObject
{
  Q_OBJECT
public:
  RemoteNode();
  virtual ~RemoteNode();
  bool switchMapper(const IMapper::RemoteType& type, const bool restore);
 // void referenceDrive(void);
  bool sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset);
  bool sendHorns(const bool active);
//  bool resetTrackDrives(void);
  const bool flagHorns(void)const{return _flagHorns;}
  bool sendDriveCommand(const ohm_schroedi_mc::DriveCtrl::Request::_command_type cmd);
private slots:
    void callBackTimerMain(void);
private:
  void callBackJoy(const sensor_msgs::Joy& joy);
  void callBackPu(const ohm_power_unit::pu& pu);
  void checkCallBacks(void);
  bool setHoming(const double homingPitch, const double homingYaw);
  void callBackQr(const ohm_perception_msgs::QrArray& qrs);

  IMapper* _mapper = NULL;
  ros::NodeHandle _nh;
  ros::Publisher _pubActiveProfile;
  ros::Publisher _pubMenuOverrideActive;
  ros::Publisher _pubReferenceCommand;
  ros::Subscriber _subsJoy;
  ros::Subscriber _subsPu;
  ros::Subscriber _subsQr;
  ros::ServiceClient _clientHeavyMetal;
  ros::ServiceClient _srvSetSensorHoming;
  ros::ServiceClient _clientFlipperPreset;
  ros::ServiceClient _clientArmHorns;
  //ros::ServiceClient _clientResetTrackDrives;
  ros::ServiceClient _clientDriveCtrl;
  bool _flagHorns;
  ros::Time _timerLastPu;
  double _homingYawFor  ;
  double _homingPitchFor;
  double _homingYawRev  ;
  double _homingPitchRev;
  QTimer* _timerMain;
  Hud _hud;
  std::vector<IMapper*> _mappers;
  double _threshES;
  bool _flagES;
  bool _flagUR;
  double _threshSwitchDir;
  ohm_perception_msgs::QrArray _qrs;
   ros::Time _qrTimer;
};

#endif /* OHM_SCHR_REMOTE_SRC_REMOTENODE_H_ */
