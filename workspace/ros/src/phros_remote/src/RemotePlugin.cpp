/*
 * RemotePlugin.cpp
 *
 *  Created on: May 7, 2019
 *      Author: phil
 */

#include "RemotePlugin.h"
#include "Communication.h"
#include "gui/Hud.h"
#include "utils/MapperPs4Pad.h"
#include "utils/MapperPsPadSing.h"
#include <memory>
#include <pluginlib/class_list_macros.h>

namespace phros_remote
{

static double _threshSwitchDir = 0.25;

RemotePlugin::RemotePlugin() {}

RemotePlugin::~RemotePlugin() { ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " alaaaaarm"); }

void RemotePlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
  auto hud = Hud::getInstance();
  context.addWidget(hud.get());
  auto comm = Communication::getInstance();
  //_timerMain = _nh.createTimer(ros::Duration(1.0 / 30.0),
  //&RemotePlugin::callBackTimerMain, this);
  connect(&_timerMain, SIGNAL(timeout(void)), this, SLOT(callBackTimerMain(void)));
  _psPad = MapperPsPadSing::instance().psPad();
  _timerMain.start(20);
}

void RemotePlugin::shutdownPlugin()
{
  auto comm = Communication::getInstance();
  comm->startStopCamera(Communication::Cams::DRIVER, false);
  comm->startStopCamera(Communication::Cams::GRIPPER, false);
}

void RemotePlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const {}

void RemotePlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings) {}

void RemotePlugin::callBackTimerMain(void) // const ros::TimerEvent& ev)
{
  // ros::spinOnce();
  // std::cout << __PRETTY_FUNCTION__ << "call" << std::endl;
  auto comm   = Communication::getInstance();
  auto mapper = MapperController::getInstance();
  auto joyMsg = comm->joyData();
  auto sysCmdMsg = comm->confData();
  //  auto imageMain = comm->imageMain();
  auto pu    = comm->power();
  auto mcRet = comm->mcRet();
  auto hud   = Hud::getInstance();
  mapper->mapImage();
  static bool initialized = false;

  if(sysCmdMsg)
  {
    mapper->switchConfig(sysCmdMsg);
    comm->clearSysCmdData();
  }
  if(joyMsg)
  {
    _psPad->map(*joyMsg);
    if(!initialized)
    {
      // std::cout << __PRETTY_FUNCTION__ << _psPad->axis(MapperPsPad::AxesPad::R2) << " / " << _psPad->axis(MapperPsPad::AxesPad::L2) << std::endl;
      if(_psPad->axis(MapperPsPad::AxesPad::R2) > AXE_MAX && _psPad->axis(MapperPsPad::AxesPad::L2) > AXE_MAX)
      // if(psPad->R2() > 0.7 && psPad->L2() > 0.7)
      // if ((joyMsg->axes[R2] > 0.7 && joyMsg->axes[L2] > 0.7) &&
      //     !initialized) // TODO: move into the pspad class
      {
        ROS_INFO("Joy Node: Initializing...");
        usleep(1000 * 1000);
        initialized = true;
        ROS_INFO("Joy Node: Initialized");
      }
      // else
      //   return;
    }
    else
      mapper->map(_psPad);
  }
  else
    initialized = false;
  //std::cout << __PRETTY_FUNCTION__ << " inform hud " << initialized << std::endl;  
  hud->setflagPadInitialized(initialized);
  // std::cout << __PRETTY_FUNCTION__ << " 2 " << std::endl;
  if(pu)
    hud->setPu(pu);
  if(mcRet)
  {
    hud->setFlipperAngles(*mcRet.get());
  }
  // if(!initialized)
  // {
  //   ROS_INFO_THROTTLE(1.0, "Joy Node: Not initialized");
  //   return;
  // }
  // std::cout << __PRETTY_FUNCTION__ << " 3 " << std::endl;

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
  // std::cout << __PRETTY_FUNCTION__ << "exit" << std::endl;
}

} // namespace phros_remote
PLUGINLIB_EXPORT_CLASS(phros_remote::RemotePlugin, rqt_gui_cpp::Plugin)
