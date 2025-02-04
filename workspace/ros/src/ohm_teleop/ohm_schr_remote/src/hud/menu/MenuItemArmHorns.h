/*
 * MenuItemArmHorns.h
 *
 *  Created on: Apr 23, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMARMHORNS_H_
#define OHM_HUD_SRC_MENU_MENUITEMARMHORNS_H_

#include "hud/menu/MenuItemBase.h"

class MenuItemArmHorns: public MenuItemBase
{
public:
  MenuItemArmHorns(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemArmHorns();
  virtual bool action(void);
private:
  bool _hornModeActive;
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMARMHORNS_H_ */
