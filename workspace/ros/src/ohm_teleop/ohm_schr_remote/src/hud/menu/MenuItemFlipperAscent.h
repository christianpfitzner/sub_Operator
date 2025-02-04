/*
 * MenuItemFlipperAscent.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_

#include "hud/menu/MenuItemBase.h"

#include <QString>

class MenuItemFlipperAscent : public MenuItemBase
{
public:
  MenuItemFlipperAscent(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperAscent();
  virtual bool action(void);
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_ */
