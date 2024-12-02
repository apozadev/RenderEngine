#pragma once

#include "Memory/GenericPool.h"
#include "Memory/PoolObjectDeleter.h"
#include "Memory/PtrTypes.h"

template<template<typename> class ElemType, size_t SIZE>
class TemplateFactory
{

  template <typename U>
  friend struct PoolObjectDeleter;

public:

  class ConstructKey
  {
    friend TemplateFactory;
    ConstructKey() {};
  };

  bool Initialize()
  {
    if (!s_oPool.IsInitialized())
    {
      s_oPool.Initialize(SIZE);
    }

    return true;
  }
  
  template <typename T>
  owner_ptr<ElemType<T>> CreateInstance()
  {
    if (!s_oPool.IsInitialized())
    {
      THROW_GENERIC_EXCEPTION("Pool not initialized")
    }
    
    ElemType<T>* pObj = (ElemType<T>*)(s_oPool.Allocate(sizeof(ElemType<T>), alignof(ElemType<T>)));
    new (pObj) ElemType<T>(ConstructKey());
    return owner_ptr<ElemType<T>>(pObj, PoolObjectDeleter<ElemType<T>>());
  }

private:

  template<class T>
  void DestroyInstance(ElemType<T>* _pInstance)
  {
    _pInstance->~ElemType<T>();
    s_oPool.Deallocate(_pInstance);
  }

  GenericPool s_oPool;

};
