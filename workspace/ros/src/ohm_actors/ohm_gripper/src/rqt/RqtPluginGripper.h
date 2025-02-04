/*
 * RqtPluginGripper.h
 *
 *  Created on: Apr 4, 2018
 *      Author: phil
 */

#ifndef OHM_GRIPPER_SRC_RQT_RQTPLUGINGRIPPER_H_
#define OHM_GRIPPER_SRC_RQT_RQTPLUGINGRIPPER_H_

#include <rqt_gui_cpp/plugin.h>
#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>

#include "ohm_gripper/ui_gripper.h"
#include "ohm_actors_msgs/ActionGripperAction.h"

#include <QWidget>

namespace ohm_gripper
{

class RqtPluginGripper : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  RqtPluginGripper();
  virtual ~RqtPluginGripper();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const;
  virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);
public slots:
  void grip(void);
  void open(void);
  void stepClose(void);
  void stepOpen(void);
private:
  QWidget* _widgetMain;
  Ui::GuiGripper* _guiUi;
  actionlib::SimpleActionClient<ohm_actors_msgs::ActionGripperAction> _as;
};

} /* namespace ohm_gripper */

#endif /* OHM_GRIPPER_SRC_RQT_RQTPLUGINGRIPPER_H_ */
