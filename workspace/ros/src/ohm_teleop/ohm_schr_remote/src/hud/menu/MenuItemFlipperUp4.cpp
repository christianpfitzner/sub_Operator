/*
 * MenuItemFlipperUp4.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemFlipperUp4.h"
#include "ohm_schroedi_mc/FlipperPreset.h"

#include <QDebug>

MenuItemFlipperUp4::MenuItemFlipperUp4(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{

}

MenuItemFlipperUp4::~MenuItemFlipperUp4()
{

}

bool MenuItemFlipperUp4::action(void)
{
  ohm_schroedi_mc::FlipperPreset preset;
  preset.request.command = ohm_schroedi_mc::FlipperPreset::Request::SMALLFPRINT;
  _parent.sendFlipperPreset(preset);
  return true;
}
