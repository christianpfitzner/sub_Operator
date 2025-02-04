/*
 * MenuItemBase.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "MenuItemBase.h"
#include <iostream>

namespace phros_remote
{

MenuItemBase::MenuItemBase(const QString& pathToPic, IMenuItem* prev, IMenuItem* next):
IMenuItem()
{
  _next = next;
  _previous = prev;
  _menuIcon = std::shared_ptr<QImage>(new QImage);
  if(!_menuIcon->load(pathToPic))
    throw std::string("Error loading pic at path ") + pathToPic.toStdString();
}

MenuItemBase::~MenuItemBase()
{
 std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
}

void MenuItemBase::pushBack(IMenuItem* item, IMenuItem* root)
{

  IMenuItem* next = &root->next();

  root->setNext(item);
  item->setPrev(root);

  next->setPrev(item);
  item->setNext(next);

//  item->setNext(&root->next());
//   item->setPrev(&root->previous());

}

void MenuItemBase::pushTop(IMenuItem* item, IMenuItem* root)
{
  IMenuItem* up = &root->up();

  root->setUp(item);
  item->setDown(root);

  item->setUp(up);
  up->setDown(item);

  item->setNext(&root->next());
  item->setPrev(&root->previous());

}


void MenuItemBase::init(IMenuItem* item)
{
  if(!item)
  {
    _next = this;
  _previous = this;
  }
  else
  {
    item->setNext(this);
    item->setPrev(this);
    _next = item;
    _previous = item;
  }
//  _up = this;
//  _down = this;
}

 void MenuItemBase::initUpDown(IMenuItem* item)
{
   if(!item)
     {
       _up = this;
     _down = this;
     }
     else
     {
       item->setUp(this);
       item->setDown(this);
       item->setNext(&this->next());
       item->setPrev(&this->previous());
       _up = item;
       _down = item;
     }
}

//void MenuItemBase::initUpDown(void)
//{
//  _up = this;
//  _down = this;
//}

}
