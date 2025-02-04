/*
 * MenuItemFlipperDescent.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERDESCENT_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERDESCENT_H_

#include "MenuItemBase.h"

class MenuItemFlipperDescent: public MenuItemBase
{
public:
  MenuItemFlipperDescent(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperDescent();
  virtual bool action(void);
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERDESCENT_H_ */
