/*
 * MenuItemModeArm.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemModeArm.h"
#include "MapperController.h"

#include <QtCore/QDebug>

namespace ohm_remote
{

MenuItemModeArm::MenuItemModeArm(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
        MenuItemBase(pathToPic, prev, next)
{

}

MenuItemModeArm::~MenuItemModeArm()
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
}

bool MenuItemModeArm::action(void)
{
  MenuItemBase::displayActionTriggered(true);
  auto mapperController = MapperController::getInstance();
  const bool retval = mapperController->switchMapper(ohm_remote::IMapper::RemoteType::ARM);
  MenuItemBase::displayActionTriggered(!retval);
  return retval;
}

}
