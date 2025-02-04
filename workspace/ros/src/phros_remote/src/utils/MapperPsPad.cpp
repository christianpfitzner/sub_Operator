#include "MapperPsPad.h"

namespace phros_remote
{
  MapperPsPad::MapperPsPad():
  _buttons(N_BUTTONS_PS),
  _axes(N_AXES, 0.0f)
  {
    
  }
  void MapperPsPad::reset()
  {
    for(auto& iter : _buttons)
      iter.reset();
    for(auto& iter : _axes) 
     iter = 0.0f; 

  }
}
