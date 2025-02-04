/*
 * RemotePlugin.h
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_REMOTEPLUGIN_H_
#define OHM_RQT_SRC_REMOTE_REMOTEPLUGIN_H_

#include <QtWidgets/QMainWindow>
#include <QtCore/QTimer>
#include <rqt_gui_cpp/plugin.h>
#include <ros/ros.h>
#include "MapperController.h"

namespace phros_remote
{
class MapperPsPad;
class RemotePlugin : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  RemotePlugin();
  virtual ~RemotePlugin();
  virtual void initPlugin(qt_gui_cpp::PluginContext &context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings &plugin_settings, qt_gui_cpp::Settings &instance_settings) const;
  virtual void restoreSettings(const qt_gui_cpp::Settings &plugin_settings, const qt_gui_cpp::Settings &instance_settings);
  public slots:
  void callBackTimerMain(void);//const ros::TimerEvent& ev);
private:
  ros::NodeHandle _nh;

  //ros::Timer _timerMain;
  QTimer _timerMain;
  std::shared_ptr<MapperPsPad> _psPad;
  //std::unique_ptr<MapperController> _mapper;
};

} // namespace ohm_remote

#endif /* OHM_RQT_SRC_REMOTE_REMOTEPLUGIN_H_ */
