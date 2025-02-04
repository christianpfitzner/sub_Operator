/*
 * IMenuItem.h
 *
 *  Created on: May 10, 2019
 *      Author: phil
 */

#ifndef OHM_RQT_SRC_REMOTE_GUI_MENU_IMENUITEM_H_
#define OHM_RQT_SRC_REMOTE_GUI_MENU_IMENUITEM_H_

#include <QtGui/QImage>
#include <memory>

namespace phros_remote
{

class IMenuItem
{
public:
  IMenuItem(void){}
  virtual ~IMenuItem(void){}
  virtual const QImage& menuIcon(void)const = 0;
  virtual bool action(void) = 0;
  virtual void pushBack(IMenuItem* item, IMenuItem* root) = 0;
  virtual void pushTop(IMenuItem* item, IMenuItem* root) = 0;
  IMenuItem& previous(void){return *_previous;}
  IMenuItem& next(void){return *_next;}
  IMenuItem& up(void){return *_up;}
  IMenuItem& down(void){return *_down;}
  void setNext(IMenuItem* item){_next = item;}
  void setPrev(IMenuItem* item){_previous = item;}
  void setUp(IMenuItem* item){_up = item;}
  void setDown(IMenuItem* item){_down = item;}
  virtual void init(IMenuItem* item) = 0;
  virtual void initUpDown(IMenuItem* item) = 0;
  virtual std::string type(void)const = 0;
protected:
  IMenuItem* _next     = nullptr;
  IMenuItem* _previous = nullptr;
  IMenuItem* _up       = nullptr;
  IMenuItem* _down     = nullptr;
  std::shared_ptr<QImage> _menuIcon;
};

}

#endif /* OHM_RQT_SRC_REMOTE_GUI_MENU_IMENUITEM_H_ */
