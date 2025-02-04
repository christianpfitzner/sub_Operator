/*
 * MapperArm.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: phil
 */

#include "MapperArm.h"
#include "MapperController.h"
#include "Hud.h"

#include "ohm_teleop_msgs/ArmCommand.h"
#include "Ps3Profiles.h"

#include <string>

#include "Hud.h"
#include "Communication.h"

namespace ohm_remote
{

MapperArm::MapperArm(ros::NodeHandle& nh):
        _nh(nh)
{
  ros::NodeHandle prvNh("~");
  std::string topicArmCommand;
  prvNh.param<std::string>("topic_arm_command", topicArmCommand, "arm_command");
  _pubArmCommand = _nh.advertise<ohm_teleop_msgs::ArmCommand>(topicArmCommand, 1);
}

MapperArm::~MapperArm()
{
  // TODO Auto-generated destructor stub
}

void MapperArm::map(const sensor_msgs::Joy& joy)
{
  auto hud = Hud::getInstance();
   hud->setArmActive(true);
  if(_reset)
  {
    _last = joy;
    //this->emptyLast();
    _reset = false;
    return;
  }
  //static sensor_msgs::Joy last = joy;

  if(joy.buttons[B_PS] && !_last.buttons[B_PS])
  {
    MapperController::getInstance()->switchMapper(IMapper::RemoteType::HUD);
    return;
  }

  ohm_teleop_msgs::ArmCommand command;
  //map axes
  command.xe = joy.axes[A1_Y];
  command.ye = joy.axes[A1_X];
  command.ze = joy.axes[A2_Y];
  //map "buttons"
  //std::cout << __PRETTY_FUNCTION__ << " cur " << joy.buttons[B_SELECT] <<  " last " << _last.buttons[B_SELECT] << std::endl;
  if(joy.buttons[B_SELECT] && !_last.buttons[B_SELECT])
  {
    command.change_coordinate_system = joy.buttons[B_SELECT];
   // abort();
  }
  //  if(joy.buttons[B_C] && !last.buttons[B_C])
  command.grzu = joy.buttons[B_C];
  //if(joy.buttons[B_S] && !last.buttons[B_S])
  command.grauf = joy.buttons[B_S];
  command.pminus = joy.buttons[B_L1];
  command.pplus = joy.buttons[B_R1];
  command.rminus = joy.buttons[B_R2];
  command.rplus  = joy.buttons[B_L2];
  command.initial_pose = joy.buttons[B_START];
  command.yminus = joy.buttons[B_LEFT];
  command.yplus = joy.buttons[B_RIGHT];
  command.initial_up = joy.buttons[B_UP];
  command.initial_look_down = joy.buttons[B_DOWN];
  command.initial_tunnel_side = joy.buttons[B_X];
  command.initial_tunnel_up = joy.buttons[B_T];
  _pubArmCommand.publish(command);
  _last = joy;

//  auto comm = Communication::getInstance();
//  auto imageMain = comm->imageMain();
//  if(imageMain)
//    {
//      hud->setImageMain(imageMain);
//     // hud->update();
//    }
}

void MapperArm::mapImage(void)
{
  auto hud = Hud::getInstance();
  auto comm = Communication::getInstance();
   auto imageGripper = comm->imageGripper();
   if(imageGripper)
     {
       hud->setImageMain(imageGripper);
      // hud->update();
     }
   else
     std::cout << __PRETTY_FUNCTION__ << "no gripper image" << std::endl;
}

}
