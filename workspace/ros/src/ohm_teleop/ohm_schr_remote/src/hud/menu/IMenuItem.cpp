/*
 * IMenuItem.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#include "IMenuItem.h"

IMenuItem::IMenuItem():
_next(NULL),
_previous(NULL),
_menuIcon(NULL),
_up(NULL),
_down(NULL)
{

}

IMenuItem::~IMenuItem()
{

}

