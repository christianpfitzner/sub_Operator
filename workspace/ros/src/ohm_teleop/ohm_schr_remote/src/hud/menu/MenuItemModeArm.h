/*
 * MenuItemModeArm.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMMODEARM_H_
#define OHM_HUD_SRC_MENU_MENUITEMMODEARM_H_

#include "MenuItemBase.h"

#include "hud/WidgetHud.h"

#include <QString>

class MenuItemModeArm: public MenuItemBase
{
public:
  MenuItemModeArm(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemModeArm();
  bool action(void);
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMMODEARM_H_ */
