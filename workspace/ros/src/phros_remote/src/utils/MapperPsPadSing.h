#ifndef MAPPER_PS_PAD_SING_H_
#define MAPPER_PS_PAD_SING_H_

#include <memory>
#include "MapperPsPad.h"

namespace phros_remote
{
  class MapperPsPadSing
  {
    public:
      virtual ~MapperPsPadSing() {}
      static MapperPsPadSing& instance();
      const std::shared_ptr<MapperPsPad>& psPad(void)const{ return _psPad; }
    private:
      MapperPsPadSing();
      static std::unique_ptr<MapperPsPadSing> _instance;
      std::shared_ptr<MapperPsPad> _psPad;
  };
}

#endif