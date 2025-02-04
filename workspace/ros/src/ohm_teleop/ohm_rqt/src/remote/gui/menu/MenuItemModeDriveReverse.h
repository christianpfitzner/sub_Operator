/*
 * MenuItemNodeDriveReverse.h
 *
 *  Created on: Apr 26, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMNODEDRIVEREVERSE_H_
#define OHM_HUD_SRC_MENU_MENUITEMNODEDRIVEREVERSE_H_

#include "MenuItemBase.h"

#include "gui/Hud.h"

namespace ohm_remote
{

class MenuItemModeDriveReverse: public MenuItemBase
{
public:
  MenuItemModeDriveReverse(const QString& pathToPic, IMenuItem* prev = nullptr, IMenuItem* next = nullptr);
  virtual ~MenuItemModeDriveReverse();
  virtual const QImage& menuIcon(void)const{return *_menuIcon;}
   virtual bool action(void);
   virtual std::string type(void)const{return "drive_reverse";}
};

}

#endif /* OHM_HUD_SRC_MENU_MENUITEMNODEDRIVEREVERSE_H_ */
