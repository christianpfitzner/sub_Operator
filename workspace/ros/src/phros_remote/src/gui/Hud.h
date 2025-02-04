/*
 * Hud.h
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_HUD_HUD_H_
#define OHM_SCHR_REMOTE_SRC_HUD_HUD_H_

#include <QtWidgets/QMainWindow>
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#include "gui/WidgetMain.h"
#include "ui_main.h"
//#include "ohm_schroedi_mc/FlipperPreset.h"
#include "ohm_schroedi_mc/mc_ret.h"
#include "ohm_power_unit/pu.h"
//#include "ohm_teleop_msgs/RemoteProfile.h"
#include "mappers/PsProfiles.h"
#include "mappers/IMapper.h"
//#include "ohm_actors_msgs/DriveState.h"
#include <vector>

#endif


namespace phros_remote
{
namespace
{
static const double GEAR_R = 6.75 * 216.0;
static const double TICS_P_TURN = 512.0;
}

class IMenuItem;
class Hud : public QMainWindow
{
  Q_OBJECT
public:
  enum class IconsConst
    {
      REMOTE_ARM = 0,
      REMOTE_DRIVE,
      REMOTE_REVERSE,
      REMOTE_INSPECT,
      ERROR,
      REFERENCE,
      YOU_SHALL_NOT_PASS
    };

  static std::shared_ptr<Hud> getInstance(void);
  virtual ~Hud();
  void initMenu(const bool serviceFlipperPresent, const bool serviceHornsPresent);
  void buttonPressed(const ButtonsPs3& button);
  void menuLeft(void) ;//{_menu = &_menu->next();}
  void menuRight(void);//{_menu = &_menu->previous();}
  void menuUp(void);
  void menuDown(void);
  void menuActive(const IMapper::RemoteType& lastType);//{_menuActive = true;}
//  void changeProfile(const ohm_teleop_msgs::RemoteProfile& profile);
  void setPu(const std::shared_ptr<ohm_power_unit::pu> pu){_guiUi->widget->setPu(pu);}
  void setCurrentProfile(const IMapper::RemoteType& type);
 // void esActive(const bool flag){_widget->setEsActive(flag);}
 // void rdRequired(const bool flag);
//  IMapper::RemoteType lastRemoteProfile(void)const{return _lastRemoteProfile;}
//  void setHeavyMetalMode(const bool active);//{_parent.sendHorns(active);}
//  const bool getHeavyMetalMode(void)const;//{return _parent.flagHorns();}
//  void setReverseMode(const bool active){_reverseMode = active;}
//  void setArmActive(const bool active){_armActive = active;}
  //const std::shared_ptr<WidgetMain> mainWidget(void)const{return _mainWidget;}
  void setImageMain(std::shared_ptr<sensor_msgs::Image> imageRos){_guiUi->widget->setImageMain(imageRos);}
  void showMenu(const bool showHide);
  void displayActionTriggered(const bool action){_guiUi->widget->setActionTriggered(action);}
  bool activateIcon(void);
  void setFlipperAngles(const ohm_schroedi_mc::mc_ret& mcRet);
 // double flipperTicksToAngle(const int32_t& ticks);
  void setGlobalCoordinateSystem(const bool cs){_guiUi->widget->setGlobalCoordinateSystem(cs);}
  void setArmConstraints(const bool val){_guiUi->widget->setArmConstraints(val);}
  void setArmActive(const bool val){_guiUi->widget->setArmActive(val);}
  void setflagPadInitialized(const bool value){_guiUi->widget->setFlagPadInitialized(value);}
public slots:
//  void sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset);
//  //void sendArmHorns(const bool& on);
//  void referenceDriveTriggered(void);
//  void spreadGripper(void);
//  void resetTrackDrives(void);
//  void setQrs(const std::vector<QrCode>& qrs){_widget->setQrs(qrs);}
  void requestArmReset(void);
private:
Hud(void);
 // void setRemoteOverride(void);
  double flipperTicksToAngle(const int32_t& ticks);
  ros::NodeHandle _nh;
  IMenuItem* _menu;
  //std::shared_ptr<IMenuItem*> _menu;
  bool _actionTriggered;
  //HudMainWindow* _widget;
  std::vector<std::shared_ptr<QImage> > _iconsConst;
  //bool _menuActive;
 // IMapper::RemoteType _lastRemoteProfile;
  bool _reverseMode;
  bool _armActive;
 // ros::Time _timerSwitchDriveMode;
 // double _switchDriveTime;
  //std::shared_ptr<WidgetMain> _mainWidget;
  static std::shared_ptr<Hud> _instance;
  std::unique_ptr<Ui::HudMain> _guiUi;
  bool _flagPadInitialized = false;
  
};


}
#endif /* OHM_SCHR_REMOTE_SRC_HUD_HUD_H_ */
