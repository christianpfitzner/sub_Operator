/*
 * MenuItemFlipperAscent.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_MENUITEMFLIPPERDESCENT_H_
#define OHM_HUD_SRC_MENU_MENUITEMFLIPPERDESCENT_H_

#include "MenuItemBase.h"

#include <QtCore/QString>

namespace phros_remote
{

class MenuItemFlipperDescent : public MenuItemBase
{
public:
  MenuItemFlipperDescent(const QString& pathToPic, IMenuItem* prev = NULL, IMenuItem* next = NULL);
  virtual ~MenuItemFlipperDescent();
  virtual bool action(void);
  virtual std::string type(void)const{return "dsc";}
};

}
#endif /* OHM_HUD_SRC_MENU_MENUITEMFLIPPERASCENT_H_ */
