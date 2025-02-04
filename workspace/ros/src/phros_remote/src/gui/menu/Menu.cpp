#include "Menu.h"
#include "MenuItemFlipperAscent.h"
#include "MenuItemFlipperDescent.h"
#include "MenuItemFlipperFlat.h"
#include "MenuItemFlipperUp4.h"
#include "MenuItemModeArm.h"
#include "MenuItemModeDrive.h"
#include "MenuItemModeInspect.h"
#include "MenuItemModeDriveReverse.h"
#include <ros/ros.h>

namespace phros_remote
{
Menu::Menu()
{
  try
  {
    std::shared_ptr<IMenuItem> object = std::make_shared<MenuItemModeDrive>(":/menu_icons/drive.png");
    _ringBuffer                       = new ItemRingBuffer<IMenuItem>(object);
    // std::cout << __PRETTY_FUNCTION__ << " initial element " << std::endl;
    // this->printConsole();
    _ringBuffer->pushBack(new ItemRingBuffer<IMenuItem>(std::make_shared<MenuItemModeDriveReverse>(":/menu_icons/drive_rev.png")));
    _ringBuffer->pushBack(new ItemRingBuffer<IMenuItem>(std::make_shared<MenuItemModeInspect>(":/menu_icons/inspection.png")));
    // std::cout << __PRETTY_FUNCTION__ << " first push" << std::endl;
    // this->printConsole();
    

    ItemRingBuffer<IMenuItem>* itemFlippers = new ItemRingBuffer<IMenuItem>(std::make_shared<MenuItemFlipperFlat>(":/menu_icons/flat.png"));
    itemFlippers->pushTop(new ItemRingBuffer<IMenuItem>(std::make_shared<MenuItemFlipperUp4>(":/menu_icons/up4.png")));
    itemFlippers->pushTop(new ItemRingBuffer<IMenuItem>(std::make_shared<MenuItemFlipperAscent>(":/menu_icons/ascent.png")));
    
    _ringBuffer->pushBack(itemFlippers);

    _ringBuffer->pushBack(new ItemRingBuffer<IMenuItem>(std::make_shared<MenuItemModeArm>(":/menu_icons/arm.png")));

  }
  catch(std::string& e)
  {
    ROS_ERROR_STREAM(__PRETTY_FUNCTION__ << " " << e << std::endl);
    std::exit(1);
  }
  // std::cout << __PRETTY_FUNCTION__ << " initialized" << std::endl;
  // this->printConsole();
  // _menu = new MenuItemModeDrive(":/menu_icons/drive.png");//   new MenuItemModeDrive(":/menu_icons/drive.png"));
  // _menu->init(new MenuItemModeDriveReverse(":/menu_icons/drive_rev.png"));
  // _menu->pushBack(new MenuItemModeArm(":/menu_icons/arm.png"), _menu);
}

Menu::~Menu() {}
IMenuItem* Menu::iconcentral() const { return &_ringBuffer->content(); }
IMenuItem* Menu::iconleft() const
{
  if(_ringBuffer->previous() != _ringBuffer)
    return &_ringBuffer->previous()->content();
  else
    return nullptr;
}
IMenuItem* Menu::iconright() const
{
  if(_ringBuffer->next() != _ringBuffer)
    return &_ringBuffer->next()->content();
  else
    return nullptr;
}

IMenuItem* Menu::icontop() const
{
  if(_ringBuffer->top() != _ringBuffer)
    return &_ringBuffer->top()->content();
  else
    return nullptr;
}

IMenuItem* Menu::iconbottom() const
{
  if(_ringBuffer->bottom() != _ringBuffer)
    return &_ringBuffer->bottom()->content();
  else
    return nullptr;
}

// namespace phros_remote

void Menu::left() { _ringBuffer = _ringBuffer->previous(); }

void Menu::right() { _ringBuffer = _ringBuffer->next(); }

void Menu::printConsole()
{
  ItemRingBuffer<phros_remote::IMenuItem>* item = _ringBuffer;
  do
  {
    std::cout << " " << item->content().type() << std::endl
              << "\t-> " << item->next()->content().type() << std::endl
              << "\t<- " << item->previous()->content().type() << std::endl;
    item = item->next();
  } while(item != _ringBuffer);
}

} // namespace phros_remote