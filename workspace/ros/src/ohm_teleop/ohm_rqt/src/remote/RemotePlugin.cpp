/*
 * RemotePlugin.cpp
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#include "RemotePlugin.h"

#include <pluginlib/class_list_macros.h>
#include "Communication.h"
#include <memory>
#include "gui/Hud.h"


namespace ohm_remote
{

static double _threshSwitchDir = 0.25;

RemotePlugin::RemotePlugin()
{
  // TODO Auto-generated constructor stub

}

RemotePlugin::~RemotePlugin()
{
  // TODO Auto-generated destructor stub
}

void RemotePlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
  auto hud = Hud::getInstance();
  context.addWidget(hud.get());
  auto comm = Communication::getInstance();
  //_timerMain = _nh.createTimer(ros::Duration(1.0 / 30.0), &RemotePlugin::callBackTimerMain, this);
 connect(&_timerMain, SIGNAL(timeout(void)), this, SLOT(callBackTimerMain(void)));
 _timerMain.start(20);
}

void RemotePlugin::shutdownPlugin()
{
  auto comm = Communication::getInstance();
  comm->startStopCamera(Communication::Cams::DRIVER, false);
  comm->startStopCamera(Communication::Cams::GRIPPER, false);
}

void RemotePlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings)const
{

}

void RemotePlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{

}

void RemotePlugin::callBackTimerMain(void)//const ros::TimerEvent& ev)
{
 // ros::spinOnce();
  std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  auto comm = Communication::getInstance();
  auto mapper = MapperController::getInstance();
  auto joyMsg = comm->joyData();
  //  auto imageMain = comm->imageMain();
  auto pu = comm->power();
  auto mcRet = comm->mcRet();
  auto hud = Hud::getInstance();
  mapper->mapImage();
  static bool initialized = false;
  if(joyMsg)
  {
    if(!initialized)
     if((joyMsg->axes[R2] > 0.7 && joyMsg->axes[L2] > 0.7) && !initialized)  //apparently the ps3 pad needs to be initialized...both accelerators pressed
    {
      ROS_INFO("Joy Node: Initializing...");
      usleep(1000 * 1000);
      initialized = true;
      ROS_INFO("Joy Node: Initialized");
    }
    mapper->map(*joyMsg.get());
   
  }
  if(pu)
    hud->setPu(pu);
  if(mcRet)
  {
    hud->setFlipperAngles(*mcRet.get());
  }
  if(!initialized)
  {
    ROS_INFO_THROTTLE(1.0, "Joy Node: Not initialized");
    return;
  }

  auto csActive = comm->csActive();
  if(csActive)
    hud->setGlobalCoordinateSystem(csActive->data);
  //  else
  //    std::cout << __PRETTY_FUNCTION__ << "nocs" << std::endl;
  auto armConstraintsReached = comm->armConstraints();
  if(armConstraintsReached)
    hud->setArmConstraints(armConstraintsReached->data);
  // std::cout << __PRETTY_FUNCTION__ << " no constraints" << std::endl;
  comm->republishImages();

  hud->update();
  std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
}

} /* namespace ohm_rqt */
PLUGINLIB_EXPORT_CLASS(ohm_remote::RemotePlugin, rqt_gui_cpp::Plugin)
