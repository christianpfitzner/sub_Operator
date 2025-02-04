/*
 * MenuItemFlipperDescent.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemFlipperDescent.h"

#include <QDebug>

MenuItemFlipperDescent::MenuItemFlipperDescent(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
MenuItemBase(pathToPic, parent, prev, next)
{

}

MenuItemFlipperDescent::~MenuItemFlipperDescent()
{

}

bool MenuItemFlipperDescent::action(void)
{
  qDebug() << __PRETTY_FUNCTION__ << " flipper in ascent mode";
  return true;
}
