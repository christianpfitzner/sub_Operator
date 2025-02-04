/*
 * MenuItemNodeDriveReverse.cpp
 *
 *  Created on: Apr 26, 2018
 *      Author: phil
 */

#include "MenuItemNodeDriveReverse.h"

MenuItemNodeDriveReverse::MenuItemNodeDriveReverse(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{
  // TODO Auto-generated constructor stub

}

MenuItemNodeDriveReverse::~MenuItemNodeDriveReverse()
{
  // TODO Auto-generated destructor stub
}

bool MenuItemNodeDriveReverse::action(void)
{
  ohm_teleop_msgs::RemoteProfile profile;
  profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_REV;
  _parent.changeProfile(profile);
  return true;
}
