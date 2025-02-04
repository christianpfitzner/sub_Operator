/*
 * MenuItemFlipperUp4.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemFlipperUp4.h"
#include "ohm_schroedi_mc/FlipperPreset.h"
#include "Communication.h"
#include "MapperController.h"
#include <QtCore/QDebug>

namespace phros_remote
{

MenuItemFlipperUp4::MenuItemFlipperUp4(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
    MenuItemBase(pathToPic, prev, next)
{

}

MenuItemFlipperUp4::~MenuItemFlipperUp4()
{

}

bool MenuItemFlipperUp4::action(void)
{
  std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
  MenuItemBase::displayActionTriggered(true);
  ohm_schroedi_mc::FlipperPreset preset;
  preset.request.command = ohm_schroedi_mc::FlipperPreset::Request::SMALLFPRINT;
  auto comm = Communication::getInstance();
  auto mapper = MapperController::getInstance();
  if(mapper->type() == IMapper::RemoteType::DRIVE_REV)
    preset.request.inverted = true;
  const bool retVal =  comm->sendFlipperPreset(preset);
  MenuItemBase::displayActionTriggered(!retVal);
  return retVal;
}

}
