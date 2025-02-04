/*
 * MenuItemModeDrive.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemModeInspect.h"
#include <QtCore/QDebug>
#include "MapperController.h"

namespace phros_remote
{

MenuItemModeInspect::MenuItemModeInspect(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
        MenuItemBase(pathToPic, prev, next)
{

}

MenuItemModeInspect::~MenuItemModeInspect()
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
}

bool MenuItemModeInspect::action(void)
{
  MenuItemBase::displayActionTriggered(true);
  auto mapperController = MapperController::getInstance();
  const bool retval = mapperController->switchMapper(phros_remote::IMapper::RemoteType::INSPECT);
  MenuItemBase::displayActionTriggered(!retval);
  return retval;
}

}
