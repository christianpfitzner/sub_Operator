#include "StateButton.h"
#include <iostream>
#include <sstream>

namespace phros_remote
{

void StateButton::update(const bool current)
{
  if(current && !_last)
    _edge = Edge::RISING;
  else if(!current && _last)
  _edge = Edge::FALLING;
  else 
  _edge = Edge::NOOP;
  _state = current;
  _last = current;        //TODO: state and last contain the same information
}

void StateButton::reset()
{
  _state = false;
  _last = false;            //TODO: state and last contain the same information
  _edge = Edge::NOOP;
}

void StateButton::print()
{
  std::stringstream ss;
  ss << "\ns = " <<_state << " " << " e = ";
  if(_edge == Edge::RISING) 
    ss << 1;
  else if(_edge == Edge::FALLING)
    ss << -1;
  else
    ss << 0;    
  ss << std::endl;
  std::cout << ss.str();

}

}