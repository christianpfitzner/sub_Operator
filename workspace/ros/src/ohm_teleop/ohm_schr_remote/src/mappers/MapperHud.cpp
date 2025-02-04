/*
 * MapperHud.cpp
 *
 *  Created on: May 6, 2018
 *      Author: phil
 */

#include "MapperHud.h"
#include "Hud.h"

MapperHud::MapperHud(Hud& hud) :
_hud(hud),
_initial(true)
{
  // TODO Auto-generated constructor stub

}

MapperHud::~MapperHud()
{
  // TODO Auto-generated destructor stub
}


void MapperHud::map(const sensor_msgs::Joy& msg)
{
  if(_initial)
  {
    _lastPs = msg;
    _initial = false;
    return;
  }
  if(msg.buttons[B_PS] && !_lastPs.buttons[B_PS])
  {
    std::cout << __PRETTY_FUNCTION__ << " rising edge ps" << std::endl;
    _hud.buttonPressed(B_PS);
  }
  else if(msg.buttons[B_LEFT] && !_lastPs.buttons[B_LEFT])
  {
    //std::cout << __PRETTY_FUNCTION__ << " left " << std::endl;
    _hud.buttonPressed(B_LEFT);
  }
  else if(msg.buttons[B_RIGHT] && !_lastPs.buttons[B_RIGHT])
  {
    //std::cout << __PRETTY_FUNCTION__ << " right " << std::endl;
    _hud.buttonPressed(B_RIGHT);
  }
  else if(msg.buttons[B_SELECT] && !_lastPs.buttons[B_SELECT])
  {
    _hud.buttonPressed(B_SELECT);
  }
  _lastPs = msg;
}

void MapperHud::reset(void)
{
  for(auto& iter : _lastPs.buttons)  //horrible horrible hack this...
    iter = true;
}
