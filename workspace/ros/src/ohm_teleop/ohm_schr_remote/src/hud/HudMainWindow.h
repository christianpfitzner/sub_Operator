/*
 * HudMainWindow.h
 *
 *  Created on: Jun 4, 2018
 *      Author: phil
 */

#ifndef OHM_SCHR_REMOTE_SRC_HUD_HUDMAINWINDOW_H_
#define OHM_SCHR_REMOTE_SRC_HUD_HUDMAINWINDOW_H_

#include <QMainWindow>
#include "ohm_schr_remote/ui_hud.h"
#ifndef Q_MOC_RUN
#include "menu/IMenuItem.h"
#include "ohm_power_unit/pu.h"
#include "ohm_teleop_msgs/RemoteProfile.h"
#include "ohm_schroedi_mc/FlipperPreset.h"
#include "ohm_perception_msgs/QrArray.h"
#endif
class Hud;
class QImage;

class HudMainWindow: public QMainWindow
{
  Q_OBJECT
public:
  HudMainWindow(Hud& hud);
  virtual ~HudMainWindow();
  void setImageSensorHead(const QImage& image){_hudUi->widget->setImageSensorHead(image);}
    void setImageGripper(const QImage& image){_hudUi->widget->setImageGripper(image);}
    void setImageThermal(const QImage& image);
    void setMenuItem(IMenuItem* item){_hudUi->widget->setMenuItem(item);}
    void setIconMode(const QImage& icon){_hudUi->widget->setIconMode(icon);}
    void setIconReference(const QImage* icon){_hudUi->widget->setIconReference(icon);}
    void setPuVals(const double current24V, const double current48V){_hudUi->widget->setPuVals(current24V, current48V);}
    void setActionTriggered(const bool val){_hudUi->widget->setActionTriggered(val);}
    void setEsActive(const bool flag){_hudUi->widget->setEsActive(flag);}
    void setFlipperAngles(std::vector<double>* const angles){_hudUi->widget->setFlipperAngles(angles);}
    void errorMessage(const QString& message);
    bool referenceDriveMenu(void);
    void setAngleGripperHead(const float angle){_hudUi->widget->setAngleGripperHead(angle);}
    void setQrs(const std::vector<QrCode>& qrs){_hudUi->widget->setQrs(qrs);}
    void paintToHud(QImage* image){_hudUi->widget->paintHudtoImage(image);}
    void setArmActive(const bool val){_hudUi->widget->setArmActive(val);}
    void setActiveCoordinateSystem(bool globalActive){_hudUi->widget->setGlobalCoordinateSystem(globalActive);}
    void setIconShallNotPass(const QImage& icon){_hudUi->widget->setIconYouShallNotPass(icon);}
    void setArmConstraintsActive(const bool val){_hudUi->widget->setArmConstraintsActive(val);}
private:
  Ui::MainWindow* _hudUi;
  Hud& _hud;
};

#endif /* OHM_SCHR_REMOTE_SRC_HUD_HUDMAINWINDOW_H_ */
