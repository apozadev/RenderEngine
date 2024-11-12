#pragma once

#include <memory>

#include "Core/TypedPool.h"

#define INIT_POOL(type) \
  namespace type##_pool_internal \
  { \
    static bool b = type::InitPool();  \
  }

template<typename T>
struct PoolObjectDeleter
{
  friend std::unique_ptr<T, PoolObjectDeleter<T>>;
private:
  void operator()(T* pObj) const
  {
    T::DestroyInstance(pObj);
  };
};

template<typename T>
using pooled_ptr = std::unique_ptr<T, PoolObjectDeleter<T>>;

template<typename T, size_t SIZE>
class PooledObject
{

  friend PoolObjectDeleter;

public:  

  class ConstructKey
  {
    friend PooledObject<T, SIZE>;
    ConstructKey() {};
  };

  PooledObject(ConstructKey) {}

  PooledObject() = delete;
  PooledObject(const PooledObject&) = delete;
  PooledObject(PooledObject&&) = delete;
  PooledObject& operator=(PooledObject&&) = delete;
  PooledObject& operator=(const PooledObject&) = delete;

  virtual ~PooledObject() = default;

  static bool InitPool()
  {
    if (!s_oPool.IsInitialized())
    {
      s_oPool.Initialize();
    }

    return true;
  }
  
  static pooled_ptr<T> CreateInstance()
  {    
    T* pObj = s_oPool.PullElement(ConstructKey{});
    return pooled_ptr<T>(pObj, PoolObjectDeleter<T>());
  }  

private:  

  static void DestroyInstance(T* pInstance)
  {
    s_oPool.ReturnElement(pInstance);
  }

  static TypedPool<T, SIZE> s_oPool;

};

template<typename T, size_t SIZE>
TypedPool<T, SIZE> PooledObject<T, SIZE>::s_oPool;
