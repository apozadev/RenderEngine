#pragma once 

#include <memory>
#include "Memory/Factory.h"

template<typename T>
struct PoolObjectDeleter
{
private:
  friend class std::unique_ptr<T, PoolObjectDeleter<T>>;

  void operator()(T* pObj) const
  {
    Factory::Destroy(pObj);
  }
};
