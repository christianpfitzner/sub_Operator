/*
 * MenuItemModeArm.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemModeArm.h"
#include "ohm_teleop_msgs/RemoteProfile.h"
#include <QDebug>

MenuItemModeArm::MenuItemModeArm(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{

}

MenuItemModeArm::~MenuItemModeArm()
{
  // TODO Auto-generated destructor stub
}

bool MenuItemModeArm::action(void)
{
  ohm_teleop_msgs::RemoteProfile profile;
  profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_ARM;
  _parent.changeProfile(profile);
  return true;
}
