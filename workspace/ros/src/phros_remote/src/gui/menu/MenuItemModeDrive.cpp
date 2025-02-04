/*
 * MenuItemModeDrive.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemModeDrive.h"
#include <QtCore/QDebug>
#include "MapperController.h"

namespace phros_remote
{

MenuItemModeDrive::MenuItemModeDrive(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
        MenuItemBase(pathToPic, prev, next)
{

}

MenuItemModeDrive::~MenuItemModeDrive()
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
}

bool MenuItemModeDrive::action(void)
{
  MenuItemBase::displayActionTriggered(true);
  auto mapperController = MapperController::getInstance();
  const bool retval = mapperController->switchMapper(phros_remote::IMapper::RemoteType::DRIVE);
  MenuItemBase::displayActionTriggered(!retval);
  return retval;
}

}
