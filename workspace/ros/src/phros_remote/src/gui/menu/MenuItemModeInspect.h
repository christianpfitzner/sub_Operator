/*
 * MenuItemModeInspect.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMMODEINSPECT_H_
#define OHM_HUD_SRC_MENU_MENUITEMMODEINSPECT_H_

#include "MenuItemBase.h"

#include "gui/Hud.h"

#include <QtCore/QString>

namespace phros_remote
{

class MenuItemModeInspect: public MenuItemBase
{
public:
  MenuItemModeInspect(const QString& pathToPic, IMenuItem* prev = nullptr, IMenuItem* next = nullptr);
  virtual ~MenuItemModeInspect();
  virtual const QImage& menuIcon(void)const{return *_menuIcon;}
  virtual bool action(void);
  virtual std::string type(void)const{return "inspect";}

};

}

#endif /* OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_ */
