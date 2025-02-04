/*
 * MenuItemBase.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemBase.h"

MenuItemBase::MenuItemBase(const QString& pathToPic, Hud& parent, IMenuItem* prev, IMenuItem* next):
IMenuItem(),
_parent(parent)
{
  _next = next;
  _previous = prev;
  _menuIcon = new QImage;
  if(!_menuIcon->load(pathToPic))
    throw std::string("Error loading pic at path ") + pathToPic.toStdString();
}

MenuItemBase::~MenuItemBase()
{
  delete _menuIcon;
}

void MenuItemBase::pushBack(IMenuItem& item, IMenuItem& root)
{
  IMenuItem* next = root.next();
  root.setNext(item);
  item.setPrev(root);
  item.setNext(*next);
  next->setPrev(item);
}

void MenuItemBase::pushTop(IMenuItem& item, IMenuItem& root)
{
  IMenuItem* up = this->up();
  this->setUp(item);
  item.setPrev(*this);
  item.setUp(*up);
  up->setDown(item);
}

void MenuItemBase::init(void)
{
  _next = this;
  _previous = this;
  _up = this;
  _down = this;
}

void MenuItemBase::initUpDown(void)
{
  _up = this;
  _down = this;
}
