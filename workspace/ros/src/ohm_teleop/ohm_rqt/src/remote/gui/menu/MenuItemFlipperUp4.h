/*
 * MenuItemFlipperUp4.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERUP4_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERUP4_H_

#include "MenuItemBase.h"

namespace ohm_remote
{

class MenuItemFlipperUp4: public MenuItemBase
{
public:
  MenuItemFlipperUp4(const QString& pathToPic, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperUp4();
  virtual bool action(void);
  virtual std::string type(void)const{return "up4";}
};

}
#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERUP4_H_ */
