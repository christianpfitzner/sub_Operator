/*
 * IMenuItem.h
 *
 *  Created on: Apr 12, 2018
 *      Author: phil
 */

#ifndef OHM_HUD_SRC_MENU_IMENUITEM_H_
#define OHM_HUD_SRC_MENU_IMENUITEM_H_

#include <QImage>

class IMenuItem
{
public:
  IMenuItem(void);
  virtual ~IMenuItem(void);
  virtual const QImage& menuIcon(void)const = 0;
  virtual bool action(void) = 0;
  virtual void pushBack(IMenuItem& item, IMenuItem& root) = 0;
  virtual void pushTop(IMenuItem& item, IMenuItem& root) = 0;
  IMenuItem* previous(void){return _previous;}
  IMenuItem* next(void){return _next;}
  IMenuItem* up(void){return _up;}
  IMenuItem* down(void){return _down;}
  void setNext(IMenuItem& item){_next = &item;}
  void setPrev(IMenuItem& item){_previous = &item;}
  void setUp(IMenuItem& item){_up = &item;}
  void setDown(IMenuItem& item){_down = &item;}
  virtual void initUpDown(void) = 0;
  virtual void init(void) = 0;
protected:
  IMenuItem* _next;
  IMenuItem* _previous;
  IMenuItem* _up;
  IMenuItem* _down;
  QImage* _menuIcon;
};



#endif /* OHM_HUD_SRC_MENU_IMENUITEM_H_ */
