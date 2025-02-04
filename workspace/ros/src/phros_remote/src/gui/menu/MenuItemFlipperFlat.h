/*
 * MenuItemFlipperFlat.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERFLAT_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERFLAT_H_

#include "MenuItemBase.h"

namespace phros_remote
{

class MenuItemFlipperFlat: public MenuItemBase
{
public:
  MenuItemFlipperFlat(const QString& pathToPic, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperFlat();
  virtual bool action(void);
  virtual std::string type(void)const{return "flat";}
};

}

#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERFLAT_H_ */
