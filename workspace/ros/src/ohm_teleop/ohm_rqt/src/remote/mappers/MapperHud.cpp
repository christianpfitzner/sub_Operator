/*
 * MapperHud.cpp
 *
 *  Created on: May 6, 2018
 *      Author: phil
 */

#include "MapperHud.h"
#include "Hud.h"
#include "MapperController.h"

namespace ohm_remote
{

MapperHud::MapperHud(void)
{
  // TODO Auto-generated constructor stub

}

MapperHud::~MapperHud()
{
  // TODO Auto-generated destructor stub
}


void MapperHud::map(const sensor_msgs::Joy& msg)
{
  if(_reset)
  {
    std::cout << __PRETTY_FUNCTION__ << "reset" << std::endl;
    _last = msg;
    //this->emptyLast();
    _reset = false;
    return;
  }
  std::shared_ptr<Hud> hud = Hud::getInstance();
  //static sensor_msgs::Joy last = msg;
  //  if(_initial)
  //  {
  //    _lastPs = msg;
  //    _initial = false;
  //    return;
  //  }
  //  if(msg.buttons[B_PS] && !_lastPs.buttons[B_PS])
  //  {
  //    std::cout << __PRETTY_FUNCTION__ << " rising edge ps" << std::endl;
  //    _hud.buttonPressed(B_PS);
  //  }
  if(msg.buttons[B_LEFT] && !_last.buttons[B_LEFT])
  {
    //std::cout << __PRETTY_FUNCTION__ << " left " << std::endl;
    hud->menuLeft();

  }
  else if(msg.buttons[B_RIGHT] && !_last.buttons[B_RIGHT])
  {
    //std::cout << __PRETTY_FUNCTION__ << " right " << std::endl;
    hud->menuRight();
  }
  else if(msg.buttons[B_SELECT] && !_last.buttons[B_SELECT])
  {
    const bool retval = hud->activateIcon();
    hud->displayActionTriggered(!retval);
  }
  else if(msg.buttons[B_PS] && !_last.buttons[B_PS])
  {
    MapperController::getInstance()->switchMapper();
  return;
  }
  else if(msg.buttons[B_UP] && !_last.buttons[B_UP])
  {
    std::cout << __PRETTY_FUNCTION__ << "up pressed" << std::endl;
    hud->menuUp();
  }
  else if(msg.buttons[B_DOWN] && !_last.buttons[B_DOWN])
    {
    std::cout << __PRETTY_FUNCTION__ << "down pressed" << std::endl;
      hud->menuDown();
    }

  _last = msg;
}

//void MapperHud::reset(void)
//{
//  for(auto& iter : _lastPs.buttons)  //horrible horrible hack this...
//    iter = true;
//}

}
