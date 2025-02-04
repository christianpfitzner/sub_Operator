/*
 * Hud.h
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_HUD_HUD_H_
#define OHM_SCHR_REMOTE_SRC_HUD_HUD_H_

#include <QObject>
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>

#include "ohm_schroedi_mc/FlipperPreset.h"
#include "ohm_schroedi_mc/mc_ret.h"
#include "ohm_power_unit/pu.h"
#include "ohm_teleop_msgs/RemoteProfile.h"
#include "mappers/Ps3Profiles.h"
#include "mappers/IMapper.h"
#include "ohm_actors_msgs/DriveState.h"

#include "ohm_perception_msgs/QrArray.h"

#include "HudMainWindow.h"
#include "WidgetHud.h"

#endif

namespace
{
static const double GEAR_R = 6.75 * 216.0;
static const double TICS_P_TURN = 512.0;
}


class RemoteNode;
class IMenuItem;
class Hud : public QObject
{
  Q_OBJECT
public:
  enum class IconsConst
    {
      REMOTE_ARM = 0,
      REMOTE_DRIVE,
      REMOTE_REVERSE,
      ERROR,
      REFERENCE,
      YOU_SHALL_NOT_PASS
    };
  Hud(RemoteNode& parent, ros::NodeHandle& nh);
  virtual ~Hud();
  void initMenu(const bool serviceFlipperPresent, const bool serviceHornsPresent);
  void buttonPressed(const Buttons_ps3_sixad& button);
  void menuActive(const IMapper::RemoteType& lastType);//{_menuActive = true;}
  void changeProfile(const ohm_teleop_msgs::RemoteProfile& profile);
  void newPu(const ohm_power_unit::pu& pu);
  void setCurrentProfile(const IMapper::RemoteType& type);
  void esActive(const bool flag){_widget->setEsActive(flag);}
  void rdRequired(const bool flag);
  IMapper::RemoteType lastRemoteProfile(void)const{return _lastRemoteProfile;}
  void setHeavyMetalMode(const bool active);//{_parent.sendHorns(active);}
  const bool getHeavyMetalMode(void)const;//{return _parent.flagHorns();}
  void setReverseMode(const bool active){_reverseMode = active;}
  void setArmActive(const bool active){_armActive = active;}
public slots:
  void sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset);
  //void sendArmHorns(const bool& on);
  void referenceDriveTriggered(void);
  void spreadGripper(void);
  void resetTrackDrives(void);
  void setQrs(const std::vector<QrCode>& qrs){_widget->setQrs(qrs);}
private:
 // void setRemoteOverride(void);
  void callBackImageSensorHead(const sensor_msgs::ImageConstPtr& img);
  void callBackImageGripper(const sensor_msgs::ImageConstPtr& img);
  void callBackImageThermal(const sensor_msgs::ImageConstPtr& img);
  void callBackRemoteProfile(const ohm_teleop_msgs::RemoteProfile& msg);
  void callBackMCret(const ohm_schroedi_mc::mc_ret& mc_ret);
  void callBackAngleGripperHead(const ohm_actors_msgs::DriveState& angle);
  void callBackInverseCoordinateSystem(const std_msgs::Bool& val);
  void callBackInverseArmInRestriction(const std_msgs::Bool& val);
  double flipperTicksToAngle(const int32_t& ticks);
  void decoceQrs(const sensor_msgs::ImageConstPtr& img, std::vector<QrCode>& qrs);
  RemoteNode& _parent;
  ros::NodeHandle& _nh;
  image_transport::ImageTransport _it;
  image_transport::Subscriber _subsCamSensorHead;
  image_transport::Subscriber _subsCamGripper;
  image_transport::Publisher _pubImageHud;
  image_transport::Publisher _pubReImageGripper;
  ros::Subscriber _subsMcRet;
  ros::Subscriber _subsAngleGripperSensorHead;
  ros::Subscriber _subsInverseCoordinateSystem;
  ros::Subscriber _subsInverseInRestriction;

 ros::Publisher _pubGripper1;
 ros::Publisher _pubGripper2;
  IMenuItem* _menu;
  bool _actionTriggered;
  HudMainWindow* _widget;
  std::vector<unsigned char>* _bufImgSensorHead;
  std::vector<unsigned char>* _bufImgGripper;
  QImage* _imgSensorHead;
  QImage* _imgGripper;
  QVector<QImage> _iconsConst;
  ohm_power_unit::pu _pu;
  //bool _menuActive;
  IMapper::RemoteType _lastRemoteProfile;
  QString _folderPics;
  bool _reverseMode;
  bool _armActive;
  ros::Time _timerSwitchDriveMode;
  double _switchDriveTime;
  float _angleGripperHead;

};

#endif /* OHM_SCHR_REMOTE_SRC_HUD_HUD_H_ */
