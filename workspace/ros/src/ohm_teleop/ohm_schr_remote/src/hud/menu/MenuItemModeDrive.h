/*
 * MenuItemModeDrive.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_
#define OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_

#include "MenuItemBase.h"

#include "hud/WidgetHud.h"

#include <QString>

class MenuItemModeDrive: public MenuItemBase
{
public:
  MenuItemModeDrive(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemModeDrive();
  virtual const QImage& menuIcon(void)const{return *_menuIcon;}
  virtual bool action(void);

};

#endif /* OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_ */
