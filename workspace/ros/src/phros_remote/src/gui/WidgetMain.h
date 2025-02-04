/*
 * WidgetMain.h
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_GUI_WIDGETMAIN_H_
#define OHM_RQT_SRC_REMOTE_GUI_WIDGETMAIN_H_

#include <QtWidgets/QWidget>
#include <QtGui/QImage>
#include <sensor_msgs/Image.h>
#include "menu/IMenuItem.h"
#include "ohm_power_unit/pu.h"
#include "PerspectiveModel.h"
#include <memory>

namespace phros_remote
{

class Hud;

class WidgetMain: public QWidget
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
  WidgetMain(QWidget* parent = NULL);
  virtual ~WidgetMain();
  void paintEvent(QPaintEvent* event);
  void setImageMain(std::shared_ptr<sensor_msgs::Image> imageRos){*_imageMain = QImage(imageRos->data.data(), imageRos->width, imageRos->height, QImage::Format_RGB888);}
  void setIconMode(std::shared_ptr<QImage> icon){_iconMode = icon;}
  void setMenu(IMenuItem* menu){_menu = menu;}
  void setActionTriggered(const bool action){_actionTriggered = action;}
  void setPu(const std::shared_ptr<ohm_power_unit::pu> pu){_power = pu;}
  void setFlipperAngles(std::vector<double>& angles){_pervModel->setAngleFlippers(&angles);}
  void setArmActive(const bool active){_armActive = active;}
  void setGlobalCoordinateSystem(const bool cs){_globalCoordinateSystem = cs;}
  void setArmConstraints(const bool val){_armConstraintsActive = val;}
  void setFlagPadInitialized(const bool value){_flagPadIntialized = value;}
private:
  void drawEnergyBar(const float& energy, const float& energyMax, const QRect& boundingRect, QPainter& painter, const BarColors colors, const QString& label, const AlarmLevels& alarm);
  std::unique_ptr<QImage> _imageMain;
  std::unique_ptr<QImage> _imageGripper;
  std::shared_ptr<QImage> _iconMode;
  IMenuItem* _menu = nullptr;
  bool _actionTriggered;
  std::shared_ptr<ohm_power_unit::pu> _power = nullptr;
  std::unique_ptr<PerspectiveModel> _pervModel;
  double _max24V;
  double _max48V;
  bool _armActive;
  bool _globalCoordinateSystem;
  bool _armConstraintsActive;
  bool _flagPadIntialized = false;
  QImage _iconCs;
  QImage _iconYouShallNotPass;
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_GUI_WIDGETMAIN_H_ */
