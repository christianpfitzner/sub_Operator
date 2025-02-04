/*
 * MenuItemFlipperAscent.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_

#include "MenuItemBase.h"

#include <QtCore/QString>

namespace ohm_remote
{

class MenuItemFlipperAscent : public MenuItemBase
{
public:
  MenuItemFlipperAscent(const QString& pathToPic, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperAscent();
  virtual bool action(void);
  virtual std::string type(void)const{return "asc";}
};

}
#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_ */
