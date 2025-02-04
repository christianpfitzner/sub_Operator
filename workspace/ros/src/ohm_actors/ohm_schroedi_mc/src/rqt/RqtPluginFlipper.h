/*
 * RqtPluginFlipper.h
 *
 *  Created on: Apr 7, 2018
 *      Author: phil
 */

#ifndef OHM_SCHROEDI_MC_SRC_RQT_RQTPLUGINFLIPPER_H_
#define OHM_SCHROEDI_MC_SRC_RQT_RQTPLUGINFLIPPER_H_

#include <rqt_gui_cpp/plugin.h>
#include <ros/ros.h>

#include "ohm_schroedi_mc/ui_flipper_ctrl.h"

#include <QWidget>

#include <string>

namespace ohm_flipper
{

class RqtPluginFlipper : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  RqtPluginFlipper();
  virtual ~RqtPluginFlipper();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const;
  virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);
public slots:
  void dialChanged(int value);
private:
  QWidget* _widgetMain;
  Ui::FlipperControl* _ctrlGuiui;
  ros::NodeHandle _nh;
  ros::Publisher _pubFlipperMessage;
  std::string _topicFlipper;
};

}
#endif /* OHM_SCHROEDI_MC_SRC_RQT_RQTPLUGINFLIPPER_H_ */
