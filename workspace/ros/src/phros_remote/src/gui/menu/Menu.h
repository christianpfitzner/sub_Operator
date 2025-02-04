#ifndef MENU_H_ 
#define MENU_H_ 

#include "ringbuffer/ItemRingBuffer.h"
#include "gui/menu/IMenuItem.h"

namespace phros_remote
{

class Menu
{
  public:
    Menu(void);
    virtual ~Menu();
    void left();
    void right();
    void top(){_ringBuffer = _ringBuffer->top();}
    void bottom(){_ringBuffer = _ringBuffer->bottom();}
    IMenuItem* iconcentral()const;
    IMenuItem* iconleft()const;
    IMenuItem* iconright()const;
    IMenuItem* icontop()const;
    IMenuItem* iconbottom()const;

    void printConsole();
  private:
    ItemRingBuffer<IMenuItem>* _ringBuffer;  
};

}

#endif