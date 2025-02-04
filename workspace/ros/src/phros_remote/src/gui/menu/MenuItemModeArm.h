/*
 * MenuItemModeArm.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMMODEARM_H_
#define OHM_HUD_SRC_MENU_MENUITEMMODEARM_H_

#include "MenuItemBase.h"

#include "gui/Hud.h"

#include <QtCore/QString>

namespace phros_remote
{

class MenuItemModeArm: public MenuItemBase
{
public:
  MenuItemModeArm(const QString& pathToPic, IMenuItem* prev = nullptr, IMenuItem* next = nullptr);
  virtual ~MenuItemModeArm();
  bool action(void);
  virtual std::string type(void)const{return "arm";}
};

}

#endif /* OHM_HUD_SRC_MENU_MENUITEMMODEARM_H_ */
