/*
 * MenuItemModeDrive.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_
#define OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_

#include "MenuItemBase.h"

#include "gui/Hud.h"

#include <QtCore/QString>

namespace ohm_remote
{

class MenuItemModeDrive: public MenuItemBase
{
public:
  MenuItemModeDrive(const QString& pathToPic, IMenuItem* prev = nullptr, IMenuItem* next = nullptr);
  virtual ~MenuItemModeDrive();
  virtual const QImage& menuIcon(void)const{return *_menuIcon;}
  virtual bool action(void);
  virtual std::string type(void)const{return "drive";}

};

}

#endif /* OHM_HUD_SRC_MENU_MENUITEMMODEDRIVE_H_ */
