#ifndef STATE_BUTTON_H_
#define STATE_BUTTON_H_

namespace phros_remote
{

class StateButton
{
public:
  enum class Edge
  {
    NOOP = 0,
    RISING,
    FALLING
  };
  StateButton() {}
  virtual ~StateButton() {}
  void        update(const bool current);
  const Edge& edge() const { return _edge; }
  void        reset();
  const bool state(void)const{return _state;}
  void print();
private:
  bool _state = false;
  Edge _edge  = Edge::NOOP;
  bool _last  = false;
};

} // namespace phros_remote

#endif