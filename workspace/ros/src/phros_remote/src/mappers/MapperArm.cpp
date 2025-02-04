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
#include "PsProfiles.h"

#include <string>

#include "Hud.h"
#include "Communication.h"

namespace phros_remote
{

MapperArm::MapperArm(ros::NodeHandle& nh, MapperController::ButtonActions* btnActions, MapperController::AxisActions* axisActions):
        _nh(nh)
{
  ros::NodeHandle prvNh("~");
  std::string topicArmCommand;
  prvNh.param<std::string>("topic_arm_command", topicArmCommand, "arm_command");
  _pubArmCommand = _nh.advertise<ohm_teleop_msgs::ArmCommand>("arm_command", 1);   //topicArmCommand

  _btnActions = btnActions;
  _axisActions = axisActions;
}

MapperArm::~MapperArm()
{
  // TODO Auto-generated destructor stub
}

void MapperArm::map(std::shared_ptr<MapperPsPad>& msg)
{
  auto hud = Hud::getInstance();
   hud->setArmActive(true);
  if(_reset)
  {
    //*_last = *msg;
    msg->reset();
    //this->emptyLast();
    _reset = false;
    return;
  }
  //static sensor_msgs::Joy last = joy;
//msg->button(MapperPsPad::ButtonsPad::PS).;


  if(msg->button(_btnActions->mapperSwitch).edge() == StateButton::Edge::RISING)
   //if(msg->buttonPressedPS() && !_last->buttonPressedPS())
  {
    MapperController::getInstance()->switchMapper(IMapper::RemoteType::HUD);
    return;
  }

  ohm_teleop_msgs::ArmCommand command;
  //map axes
  //original
  //command.xe = msg->axis(MapperPsPad::AxesPad::STICK_LEFT_Y);//    stickLeftY();//joy.axes[A1_Y];
  //command.ye = msg->axis(MapperPsPad::AxesPad::STICK_LEFT_X);//stickLeftX();//joy.axes[A1_X];
  //command.ze = msg->axis(MapperPsPad::AxesPad::STICK_RIGHT_Y);//_msg->stickRightY();//joy.axes[A2_Y];
  //volli
  command.xe = msg->axis(_axisActions->armFwBw);//    stickLeftY();//joy.axes[A1_Y];
  command.ye = msg->axis(_axisActions->armLeftRight);//stickLeftX();//joy.axes[A1_X];
  command.ze = msg->axis(_axisActions->armUpDown);//_msg->stickRightY();//joy.axes[A2_Y];
  //map "buttons"
  //std::cout << __PRETTY_FUNCTION__ << " cur " << joy.buttons[B_SELECT] <<  " last " << _last.buttons[B_SELECT] << std::endl;
  //if(joy.buttons[B_SELECT] && !_last.buttons[B_SELECT])
  if(msg->button(_btnActions->changeCord).edge() == StateButton::Edge::RISING)
  //if(msg->buttonPressedSL() && !_last->buttonPressedSL())
  {
    //command.change_coordinate_system = joy.buttons[B_SELECT];
    command.change_coordinate_system = true;
   // abort();
  }
  /*//  if(joy.buttons[B_C] && !last.buttons[B_C])
  command.grzu = msg->button(MapperPsPad::ButtonsPad::C).state();//->buttonPressedC();//joy.buttons[B_C];  //TODO maybe put edge detection info MapperPsPadclass
  //if(joy.buttons[B_S] && !last.buttons[B_S])
  command.grauf = msg->button(MapperPsPad::ButtonsPad::S).state();//msg->buttonPressedS();//joy.buttons[B_S];
  command.pminus = msg->button(MapperPsPad::ButtonsPad::L1).state();//msg->buttonPressedL1();//joy.buttons[B_L1];
  command.pplus = msg->button(MapperPsPad::ButtonsPad::R1).state();//msg->buttonPressedR1();//joy.buttons[B_R1];
  command.rminus = msg->button(MapperPsPad::ButtonsPad::L2).state();//msg->buttonPressedR2();//joy.buttons[B_R2];
  command.rplus  = msg->button(MapperPsPad::ButtonsPad::R2).state();//msg->buttonPressedL2();//joy.buttons[B_L2];
  command.initial_pose = msg->button(MapperPsPad::ButtonsPad::ST).state();//msg->buttonPressedST();//joy.buttons[B_START];
  command.yminus = msg->button(MapperPsPad::ButtonsPad::CROSS_LEFT).state();//msg->crossLeft();//joy.buttons[B_LEFT];
  command.yplus = msg->button(MapperPsPad::ButtonsPad::CROSS_RIGHT).state();//msg->crossRight();//joy.buttons[B_RIGHT];
  command.initial_up = msg->button(MapperPsPad::ButtonsPad::CROSS_UP).state();//msg->crossUp();//joy.buttons[B_UP];
  command.initial_look_down = msg->button(MapperPsPad::ButtonsPad::CROSS_DOWN).state();//msg->crossDown();//joy.buttons[B_DOWN];
  //command.initial_tunnel_side = msg->button(MapperPsPad::ButtonsPad::X).state();//msg->buttonPressedX();//joy.buttons[B_X];
  //command.initial_tunnel_up = msg->button(MapperPsPad::ButtonsPad::T).state();//msg->buttonPressedT();//joy.buttons[B_T];*/
  
    command.grzu = msg->button(_btnActions->closeGrp).state();//->buttonPressedC();//joy.buttons[B_C];  //TODO maybe put edge detection info MapperPsPadclass
  //if(joy.buttons[B_S] && !last.buttons[B_S])
  command.grauf = msg->button(_btnActions->openGrp).state();//msg->buttonPressedS();//joy.buttons[B_S];
  command.pminus = msg->button(_btnActions->rotWrstL).state();//msg->buttonPressedL1();//joy.buttons[B_L1];
  command.pplus = msg->button(_btnActions->rotWrstR).state();//msg->buttonPressedR1();//joy.buttons[B_R1];
  command.rplus = msg->button(_btnActions->rotGrpR).state();//msg->buttonPressedR2();//joy.buttons[B_R2];
  command.rminus  = msg->button(_btnActions->rotGrpL).state();//msg->buttonPressedL2();//joy.buttons[B_L2];
  command.initial_pose = msg->button(_btnActions->foldArm).state();//msg->buttonPressedST();//joy.buttons[B_START];
  command.yminus = msg->button(_btnActions->rotWrstU).state();//msg->crossLeft();//joy.buttons[B_LEFT];
  command.yplus = msg->button(_btnActions->rotWrstD).state();//msg->crossRight();//joy.buttons[B_RIGHT];
  command.initial_up = msg->button(_btnActions->homePos).state();//msg->crossUp();//joy.buttons[B_UP];
  command.initial_look_down = msg->button(_btnActions->lookDown).state();//msg->crossDown();//joy.buttons[B_DOWN];
  //command.initial_tunnel_side = msg->button(MapperPsPad::ButtonsPad::X).state();//msg->buttonPressedX();//joy.buttons[B_X];
  //command.initial_tunnel_up = msg->button(MapperPsPad::ButtonsPad::T).state();//msg->buttonPressedT();//joy.buttons[B_T];
  _pubArmCommand.publish(command);
  //*_last = *msg;

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
     ROS_ERROR_STREAM_THROTTLE(1.0, __PRETTY_FUNCTION__ << " no image received");
}

}
