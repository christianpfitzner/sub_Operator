/*
 * MenuItemFlipperAscent.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMARMMODEIDLE_H_
#define OHM_HUD_SRC_MENU_MENUITEMARMMODEIDLE_H_

#include "MenuItemBase.h"

#include <QtCore/QString>

namespace phros_remote
{

class MenuItemArmInitialIdle : public MenuItemBase
{
public:
  MenuItemArmInitialIdle(const QString& pathToPic, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemArmInitialIdle();
  virtual bool action(void);
  virtual std::string type(void)const{return "arm_init_idle";}
};

}
#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_ */
