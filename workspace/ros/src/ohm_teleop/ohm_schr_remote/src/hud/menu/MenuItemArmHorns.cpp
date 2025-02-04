/*
 * MenuItemArmHorns.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: phil
 */

#include "MenuItemArmHorns.h"

MenuItemArmHorns::MenuItemArmHorns(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next),
_hornModeActive(true)
{
  // TODO Auto-generated constructor stub

}

MenuItemArmHorns::~MenuItemArmHorns()
{

}

bool MenuItemArmHorns::action(void)
{
  bool request = !_parent.getHeavyMetalMode();
  _parent.setHeavyMetalMode(request);
  return true;

//  if(_parent.setHeavyMetalMode(!_hornModeActive))
//    _hornModeActive = !_hornModeActive;
//  else
//    return false;
//  return true;
}
