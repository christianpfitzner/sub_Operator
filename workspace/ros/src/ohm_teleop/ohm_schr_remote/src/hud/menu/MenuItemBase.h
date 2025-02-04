/*
 * MenuItemBase.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMBASE_H_
#define OHM_HUD_SRC_MENU_MENUITEMBASE_H_

#include "IMenuItem.h"
#include "hud/Hud.h"

class MenuItemBase: public IMenuItem
{
public:
  MenuItemBase(const QString& pathToPic, Hud& parent, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemBase();
  virtual const QImage& menuIcon(void)const{return *_menuIcon;}
  virtual bool action(void) = 0;
  virtual void pushBack(IMenuItem& item, IMenuItem& root);
  virtual void pushTop(IMenuItem& item, IMenuItem& root);
  virtual void initUpDown(void);
  void init(void);
protected:
  Hud& _parent;
private:


};

#endif /* OHM_HUD_SRC_MENU_MENUITEMBASE_H_ */
