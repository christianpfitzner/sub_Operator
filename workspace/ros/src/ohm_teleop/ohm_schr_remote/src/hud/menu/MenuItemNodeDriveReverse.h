/*
 * MenuItemNodeDriveReverse.h
 *
 *  Created on: Apr 26, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMNODEDRIVEREVERSE_H_
#define OHM_HUD_SRC_MENU_MENUITEMNODEDRIVEREVERSE_H_

#include "MenuItemBase.h"

#include "WidgetHud.h"

class MenuItemNodeDriveReverse: public MenuItemBase
{
public:
  MenuItemNodeDriveReverse(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemNodeDriveReverse();
  virtual const QImage& menuIcon(void)const{return *_menuIcon;}
   virtual bool action(void);
};

#endif /* OHM_HUD_SRC_MENU_MENUITEMNODEDRIVEREVERSE_H_ */
