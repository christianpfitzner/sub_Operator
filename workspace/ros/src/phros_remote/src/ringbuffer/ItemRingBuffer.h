#ifndef ITEMRINGBUFFER_H_
#define ITEMRINGBUFFER_H_
#include <memory>

template<class T> class ItemRingBuffer
{
public:
  ItemRingBuffer(std::shared_ptr<T> object);
  ItemRingBuffer<T>* next() const { return _next; }
  ItemRingBuffer<T>* previous(void) const { return _previous; }
  ItemRingBuffer<T>* top() const { return _top; }
  ItemRingBuffer<T>* bottom() const { return _bottom; }
  void            setNext(ItemRingBuffer<T>* item) { _next = item; }
  void            setPrevious(ItemRingBuffer<T>* item) { _previous = item; }
  void            setTop(ItemRingBuffer<T>* item) { _top = item; }
  void            setBottom(ItemRingBuffer<T>* item) { _bottom = item; }
  void pushBack(ItemRingBuffer<T>* itemNew);
  void pushTop(ItemRingBuffer<T>* itemNew);
  const T& content(void)const{return *_object;}
  T& content(void){return *_object;}
private:
  std::shared_ptr<T> _object;
  ItemRingBuffer<T>*          _next;
  ItemRingBuffer<T>*          _previous;
  ItemRingBuffer<T>*          _top;
  ItemRingBuffer<T>*          _bottom;
};

template<class T>
ItemRingBuffer<T>::ItemRingBuffer(std::shared_ptr<T> object):
  _object(object),
  _next(this),
  _previous(this),
  _top(this),
  _bottom(this)
{

}
template<class T>
void ItemRingBuffer<T>::pushBack(ItemRingBuffer<T>* itemNew)
{
  ItemRingBuffer<T>* nextOld = _next;
  _next = itemNew;
  itemNew->setPrevious(this);
  itemNew->setNext(nextOld);
  nextOld->setPrevious(itemNew);
  if(_previous == this)
    _previous = itemNew;
}

template<class T>
void ItemRingBuffer<T>::pushTop(ItemRingBuffer<T>* itemNew)
{
  ItemRingBuffer<T>* topOld = _top; 
  _top = itemNew;
  itemNew->setBottom(this);
  itemNew->setTop(topOld);
  topOld->setBottom(itemNew);
  if(_bottom == this)
    _bottom = itemNew;
}

#endif