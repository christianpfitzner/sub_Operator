/*
 * MenuItemFlipperAscent.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemArmInitialLowered.h"
#include "MapperController.h"
#include "Communication.h"
#include <QtCore/QDebug>

namespace phros_remote
{

MenuItemArmInitialLowered::MenuItemArmInitialLowered(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
    MenuItemBase(pathToPic, prev, next)
{
}

MenuItemArmInitialLowered::~MenuItemArmInitialLowered()
{

}

bool MenuItemArmInitialLowered::action(void)
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
  MenuItemBase::displayActionTriggered(true);
  // ohm_schroedi_mc::FlipperPreset preset;
  // preset.request.command = ohm_schroedi_mc::FlipperPreset::Request::ASCEND;
  // auto comm = Communication::getInstance();
  // auto mapper = MapperController::getInstance();
  // if(mapper->type() == IMapper::RemoteType::DRIVE_REV)
  //   preset.request.inverted = true;
  // const bool retVal =  comm->sendFlipperPreset(preset);
  // MenuItemBase::displayActionTriggered(!retVal);
  const bool retVal = false;
  return retVal;
}

}
