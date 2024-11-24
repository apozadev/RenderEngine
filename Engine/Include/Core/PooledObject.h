#pragma once

#include <memory>

#include "Core/TypedPool.h"
#include "Core/GenericPool.h"

#define INIT_POOL(type) \
  namespace type##_pool_internal \
  { \
    static bool b = type::GetFactory()->InitPool();  \
  }

template<typename T>
struct PoolObjectDeleter
{
  friend std::unique_ptr<T, PoolObjectDeleter<T>>;

  void operator()(T* pObj) const
  {
    T::GetFactory()->DestroyInstance(pObj);
  };
};

template<typename T>
using pooled_ptr = std::unique_ptr<T, PoolObjectDeleter<T>>;

template<typename T, class POOL_TYPE>
class PooledObject
{    

public:

  class ConstructKey
  {
    friend PooledObject<T, POOL_TYPE>;
    friend PooledObject<T, POOL_TYPE>::Factory;
    ConstructKey() {};
  };

  class Factory
  {    
    friend PooledObject<T, POOL_TYPE>;
    friend PoolObjectDeleter<T>;

  public:

    bool InitPool()
    {
      if (!s_oPool.IsInitialized())
      {
        s_oPool.Initialize();
      }

      return true;
    }

    pooled_ptr<T> CreateInstance()
    {

      if (!s_oPool.IsInitialized())
      {
        THROW_GENERIC_EXCEPTION("Pool not initialized")
      }

      T* pObj = static_cast<T*>(s_oPool.PullElement(ConstructKey{}));
      return pooled_ptr<T>(pObj, PoolObjectDeleter<T>());
    }

  private:

    void DestroyInstance(T* _pInstance)
    {
      s_oPool.ReturnElement(_pInstance);
    }

    POOL_TYPE s_oPool;

  };

  PooledObject(ConstructKey) {}

  PooledObject() = delete;
  PooledObject(const PooledObject&) = delete;
  PooledObject(PooledObject&&) = delete;
  PooledObject& operator=(PooledObject&&) = delete;
  PooledObject& operator=(const PooledObject&) = delete;

  virtual ~PooledObject() = default;

  static Factory* GetFactory()
  {
    return &m_oFactory;
  }

private: 

  static Factory m_oFactory;

};

template<typename T, size_t SIZE>
using TypedPooledObject = PooledObject<T, TypedPool<T, SIZE>>;

template<typename T, class POOL_TYPE>
typename PooledObject<T, POOL_TYPE>::Factory PooledObject<T, POOL_TYPE>::m_oFactory;

