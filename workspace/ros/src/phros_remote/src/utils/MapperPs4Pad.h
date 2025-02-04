#ifndef MAPPERPS4PAD_H_
#define MAPPERPS4PAD_H_

#include "MapperPsPad.h"
#include <memory>

namespace phros_remote 
{

namespace
{
  const float AXE_MAX = 0.99f;
}

class MapperPs4Pad : public MapperPsPad {
public:
  MapperPs4Pad() {}
  virtual ~MapperPs4Pad() {std::cout << __PRETTY_FUNCTION__ << " alaaaaarm " << std::endl;}
  virtual void map(const sensor_msgs::Joy &joy);
  virtual void print();
private:
  
};

} // namespace phros_remote

#endif