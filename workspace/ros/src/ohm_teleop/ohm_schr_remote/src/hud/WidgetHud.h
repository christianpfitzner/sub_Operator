/*
 * WidgetHud.h
 *
 *  Created on: Apr 24, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_HUD_WIDGETHUD_H_
#define OHM_SCHR_REMOTE_SRC_HUD_WIDGETHUD_H_

#include <QMainWindow>
#ifndef Q_MOC_RUN
#include "menu/IMenuItem.h"
#include "ohm_power_unit/pu.h"
#include "ohm_teleop_msgs/RemoteProfile.h"
#include "ohm_schroedi_mc/FlipperPreset.h"
#include "ohm_perception_msgs/QrArray.h"
#endif
#include "PerspectiveModel.h"
class Hud;
class QImage;

struct QrCode
{
  QPoint center;
  QRect rect;
  std::vector<QPoint> corners;
  std::string message;
};


class WidgetHud: public QMainWindow
{
  Q_OBJECT
public:
  struct BarColors
  {
    BarColors(const QColor& ok, const QColor& warning, const QColor& alarm):
      ok(ok),
      warning(warning),
      alarm(alarm){}
    const QColor ok;
    const QColor warning;
    const QColor alarm;
  };
  struct AlarmLevels
  {
    AlarmLevels(const float warning, const float alarm):
      warning(warning),
      alarm(alarm){}
    const float warning;
    const float alarm;
  };

  WidgetHud(QWidget* parent = NULL);
  virtual ~WidgetHud();
  virtual void paintEvent(QPaintEvent* event);
  void drawEnergyBar(const float& energy, const float& energyMax, const QRect& boundingRect, QPainter& painter, const BarColors colors, const QString& label, const AlarmLevels& alarm);
  void setImageSensorHead(const QImage& image);
  void setImageGripper(const QImage& image){_imgGripper = &image;}
  void setImageThermal(const QImage& image);
  void setMenuItem(IMenuItem* item){_menu = item;}
  void setIconMode(const QImage& icon){_iconMode = &icon;}
  void setIconReference(const QImage* icon){_iconReference = icon;}
  void setPuVals(const double current24V, const double current48V);
  void setActionTriggered(const bool val){_actionTriggered = val;}
  void setEsActive(const bool flag){_esActive = flag;}
  void setFlipperAngles(std::vector<double>* const angles){_pervModel->setAngleFlippers(angles);}
  void setAngleGripperHead(const float angle){_angleGripperHead = angle;}
  bool setDrawAngleGripperHead(const bool newVal){return _drawAngleGripperHead = newVal;}
  void setQrs(const std::vector<QrCode>& qrs){_qrs = qrs;}
  void paintHudtoImage(QImage* image);
  void setArmActive(const bool val){_armActive = val;}
  void setGlobalCoordinateSystem(const bool active){_globalCoordinateSystem = active;}
  void setIconYouShallNotPass(const QImage& icon){_iconShallNotPass = &icon;}
  void setArmConstraintsActive(const bool val){_armConstraintsActive = val;}
signals:
  void sendFlipperPreset(const ohm_schroedi_mc::FlipperPreset& preset);
  void sendHorns(const bool& active);
  void changeProfile(const ohm_teleop_msgs::RemoteProfile& profile);
private:
  //Hud* _parent;
  IMenuItem* _menu;
  const QImage* _imgSensorHead;
  const QImage* _imgGripper;
  const QImage* _iconMode;
  const QImage* _iconReference;
  const QImage* _iconShallNotPass;
  QImage* _iconPi;
  double _current24V;
  double _max24V;
  double _current48V;
  double _max48V;
  bool _actionTriggered;
  bool _initialized;
  bool _esActive;
  PerspectiveModel* _pervModel;
  float _angleGripperHead;
  bool _drawAngleGripperHead;
  std::vector<QrCode> _qrs;
  bool _armActive;
  bool _globalCoordinateSystem;
  bool _armConstraintsActive;
  };

#endif /* OHM_SCHR_REMOTE_SRC_HUD_WIDGETHUD_H_ */
