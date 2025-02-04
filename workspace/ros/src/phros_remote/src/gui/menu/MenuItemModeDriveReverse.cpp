/*
 * MenuItemNodeDriveReverse.cpp
 *
 *  Created on: Apr 26, 2018
 *      Author: phil
 */

#include "MenuItemModeDriveReverse.h"
#include "MapperController.h"

namespace phros_remote
{

MenuItemModeDriveReverse::MenuItemModeDriveReverse(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
        MenuItemBase(pathToPic, prev, next)
{
  // TODO Auto-generated constructor stub

}

MenuItemModeDriveReverse::~MenuItemModeDriveReverse()
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
}

bool MenuItemModeDriveReverse::action(void)
{
  MenuItemBase::displayActionTriggered(true);
  auto mapperController = MapperController::getInstance();
  const bool retval = mapperController->switchMapper(phros_remote::IMapper::RemoteType::DRIVE_REV);
  MenuItemBase::displayActionTriggered(!retval);
  return retval;
}

}
