/*
 * MenuItemModeDrive.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemModeDrive.h"
#include "ohm_teleop_msgs/RemoteProfile.h"
#include <QDebug>

MenuItemModeDrive::MenuItemModeDrive(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{

}

MenuItemModeDrive::~MenuItemModeDrive()
{
  delete _menuIcon;
}

bool MenuItemModeDrive::action(void)
{
  ohm_teleop_msgs::RemoteProfile profile;
  profile.remote = ohm_teleop_msgs::RemoteProfile::MAP_DRIVE;
  _parent.changeProfile(profile);
  return true;
}
