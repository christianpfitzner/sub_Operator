/*
 * MenuItemFlipperAscent.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemFlipperAscent.h"

#include <QDebug>

MenuItemFlipperAscent::MenuItemFlipperAscent(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{
}

MenuItemFlipperAscent::~MenuItemFlipperAscent()
{
  delete _menuIcon;
}

bool MenuItemFlipperAscent::action(void)
{
  ohm_schroedi_mc::FlipperPreset preset;
    preset.request.command = ohm_schroedi_mc::FlipperPreset::Request::ASCEND;
  _parent.sendFlipperPreset(preset);
    return true;
}
