/*
 * MapperHud.cpp
 *
 *  Created on: May 6, 2018
 *      Author: phil
 */

#include "MapperHud.h"
#include "Hud.h"
#include "MapperController.h"

namespace phros_remote
{

MapperHud::MapperHud(void)
{
  // TODO Auto-generated constructor stub

}

MapperHud::~MapperHud()
{
  // TODO Auto-generated destructor stub
}


void MapperHud::map(std::shared_ptr<MapperPsPad>& msg)
{
  if(_reset)
  {
    
    //std::cout << __PRETTY_FUNCTION__ << " reset " << std::endl;
    //this->emptyLast();
    //msg->reset();  //TODO: this call spoils the edge detection. The reset should be alled in case the joy msg times out... not after switch mapper
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
  if(msg->button(MapperPsPad::ButtonsPad::CROSS_LEFT).edge() == StateButton::Edge::RISING)
  //if(msg->crossLeft() && !_last->crossLeft())
  //if(msg.buttons[B_LEFT] && !_last.buttons[B_LEFT])
  {
    //std::cout << __PRETTY_FUNCTION__ << " left " << std::endl;
    hud->menuLeft();

  }
  else if(msg->button(MapperPsPad::ButtonsPad::CROSS_RIGHT).edge() == StateButton::Edge::RISING)
  //else if(msg->crossRight() && !_last->crossRight())
  //else if(msg.buttons[B_RIGHT] && !_last.buttons[B_RIGHT])
  {
    //std::cout << __PRETTY_FUNCTION__ << " right " << std::endl;
    hud->menuRight();
  }
  else if(msg->button(MapperPsPad::ButtonsPad::SL).edge() == StateButton::Edge::RISING)
  //else if(msg->buttonPressedSL() && !_last->buttonPressedSL())
  //else if(msg.buttons[B_SELECT] && !_last.buttons[B_SELECT])
  {
    const bool retval = hud->activateIcon();
    hud->displayActionTriggered(!retval);
  }
  else if(msg->button(MapperPsPad::ButtonsPad::PS).edge() == StateButton::Edge::RISING)
  //else if(msg->buttonPressedPS() && !_last->buttonPressedPS())
  //else if(msg.buttons[B_PS] && !_last.buttons[B_PS])
  {
    //std::cout << __PRETTY_FUNCTION__ << " switch back " << std::endl;
    MapperController::getInstance()->switchMapper();
  return;
  }
  else if(msg->button(MapperPsPad::ButtonsPad::CROSS_UP).edge() == StateButton::Edge::RISING)
  //else if(msg->crossUp() && !_last->crossUp())
//  else if(msg.buttons[B_UP] && !_last.buttons[B_UP])
  {
    //std::cout << __PRETTY_FUNCTION__ << "up pressed" << std::endl;
    hud->menuUp();
  }
  else if(msg->button(MapperPsPad::ButtonsPad::CROSS_DOWN).edge() == StateButton::Edge::RISING)
  //else if(msg->crossDown() && !_last->crossDown())
  //else if(msg.buttons[B_DOWN] && !_last.buttons[B_DOWN])
    {
    //std::cout << __PRETTY_FUNCTION__ << "down pressed" << std::endl;
      hud->menuDown();
    }

  //*_last = *msg;
}

//void MapperHud::reset(void)
//{
//  for(auto& iter : _lastPs.buttons)  //horrible horrible hack this...
//    iter = true;
//}

}
