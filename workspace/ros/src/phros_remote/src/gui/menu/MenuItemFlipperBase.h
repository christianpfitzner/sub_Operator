/*
 * MenuItemFlipperBase.h
 *
 *  Created on: May 29, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_GUI_MENU_MENUITEMFLIPPERBASE_H_
#define OHM_RQT_SRC_REMOTE_GUI_MENU_MENUITEMFLIPPERBASE_H_

#include "MenuItemBase.h"

namespace phros_remote
{

class MenuItemFlipperBase: public MenuItemBase
{
public:
  MenuItemFlipperBase(const QString& pathToPic, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperBase();
  virtual bool action(void);
};

} /* namespace ohm_remote */

#endif /* OHM_RQT_SRC_REMOTE_GUI_MENU_MENUITEMFLIPPERBASE_H_ */
