/*
 * MenuItemBase.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMBASE_H_
#define OHM_HUD_SRC_MENU_MENUITEMBASE_H_

#include "IMenuItem.h"
#include "gui/Hud.h"

namespace ohm_remote
{

class MenuItemBase: public IMenuItem
{
public:
  MenuItemBase(const QString& pathToPic, IMenuItem* prev = nullptr, IMenuItem* next = nullptr);
  virtual ~MenuItemBase();
  virtual const QImage& menuIcon(void)const{return *_menuIcon.get();}
  virtual bool action(void) = 0;
  virtual void pushBack(IMenuItem* item, IMenuItem* root);
  void displayActionTriggered(const bool action){Hud::getInstance()->displayActionTriggered(action);}
  virtual void pushTop(IMenuItem* item, IMenuItem* root);
 // virtual void initUpDown(void);
  virtual void init(IMenuItem* item = nullptr);
  virtual void initUpDown(IMenuItem* item = nullptr);

};

}

#endif /* OHM_HUD_SRC_MENU_MENUITEMBASE_H_ */
