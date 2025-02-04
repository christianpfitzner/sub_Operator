/*
 * MenuItemFlipperUp4.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERUP4_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERUP4_H_

#include "MenuItemBase.h"

class MenuItemFlipperUp4: public MenuItemBase
{
public:
  MenuItemFlipperUp4(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperUp4();
  virtual bool action(void);
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERUP4_H_ */
