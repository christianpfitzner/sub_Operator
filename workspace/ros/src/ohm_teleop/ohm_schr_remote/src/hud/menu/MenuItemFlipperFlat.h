/*
 * MenuItemFlipperFlat.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERFLAT_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERFLAT_H_

#include "MenuItemBase.h"

class MenuItemFlipperFlat: public MenuItemBase
{
public:
  MenuItemFlipperFlat(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperFlat();
  virtual bool action(void);
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERFLAT_H_ */
