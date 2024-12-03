#pragma once 

#include <memory>

#include "Memory/PoolObjectDeleter.h"

template<typename T>
class owner_ptr : public std::unique_ptr<T, PoolObjectDeleter<T>>
{
public:

  using std::unique_ptr<T, PoolObjectDeleter<T>>::unique_ptr;

  template<typename U>
  owner_ptr<U> cast_release()
  {
    return owner_ptr<RenderStep>(static_cast<RenderStep*>(release()));
  }
};