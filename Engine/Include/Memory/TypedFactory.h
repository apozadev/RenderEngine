#pragma once

#include "Memory/TypedPool.h"
#include "Memory/PoolObjectDeleter.h"
#include "Memory/PtrTypes.h"

template<typename T, size_t SIZE>
class TypedFactory
{  
  friend struct PoolObjectDeleter<T>;

public:

  class ConstructKey
  {
    friend TypedFactory;
    ConstructKey() {};
  };

  bool Initialize()
  {
    if (!s_oPool.IsInitialized())
    {
      s_oPool.Initialize();
    }

    return true;
  }

  owner_ptr<T> CreateInstance()
  {

    if (!s_oPool.IsInitialized())
    {
      THROW_GENERIC_EXCEPTION("Pool not initialized")
    }

    T* pObj = s_oPool.PullElement(ConstructKey{});
    return owner_ptr<T>(pObj, PoolObjectDeleter<T>());
  }

private:

  void DestroyInstance(T* _pInstance)
  {
    s_oPool.ReturnElement(_pInstance);
  }

  TypedPool<T, SIZE> s_oPool;

};
