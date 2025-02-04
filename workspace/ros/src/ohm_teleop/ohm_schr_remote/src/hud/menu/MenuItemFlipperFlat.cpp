/*
 * MenuItemFlipperFlat.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemFlipperFlat.h"

#include <QDebug>

MenuItemFlipperFlat::MenuItemFlipperFlat(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{

}

MenuItemFlipperFlat::~MenuItemFlipperFlat()
{

}

bool MenuItemFlipperFlat::action(void)
{
  ohm_schroedi_mc::FlipperPreset preset;
  preset.request.command = ohm_schroedi_mc::FlipperPreset::Request::FLAT;
  _parent.sendFlipperPreset(preset);
  return true;
}
