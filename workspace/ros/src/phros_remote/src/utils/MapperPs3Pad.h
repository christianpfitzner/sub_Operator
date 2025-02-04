#ifndef MAPPERPS3PAD_H_
#define MAPPERPS3PAD_H_

#include "MapperPsPad.h"
#include <memory>
#include <iostream>

namespace
{
  const float AXE_MAX = 0.99f;
}

namespace phros_remote 
{
  class MapperPs3Pad : public MapperPsPad
  {
    public:
    MapperPs3Pad(){std::cout << __PRETTY_FUNCTION__ << " ps3 " << std::endl;}
    virtual ~MapperPs3Pad(){}
    virtual void map(const sensor_msgs::Joy &joy);
    virtual void print();
    private:
  };
}

#endif