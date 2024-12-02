#pragma once

#include <memory>

#include "Memory/TypedFactory.h"
#include "Memory/TemplateFactory.h"

#define INIT_POOL(type) \
  namespace type##_pool_internal \
  { \
    static bool b = type::GetFactory()->Initialize();  \
  }

#define INIT_POOL_TEMPLATE(type) \
  namespace type##_pool_internal \
  { \
    static bool b = type<char>::GetFactory()->Initialize();  \
  }

template<class FACTORY_TYPE>
class PooledObject
{    

public:  

  PooledObject(typename FACTORY_TYPE::ConstructKey) {}

  PooledObject() = delete;
  PooledObject(PooledObject&) = delete;
  PooledObject(const PooledObject&) = delete;
  PooledObject(PooledObject&&) = delete;
  PooledObject& operator=(PooledObject&&) = delete;
  PooledObject& operator=(const PooledObject&) = delete;

  virtual ~PooledObject() = default;

  static FACTORY_TYPE* GetFactory()
  {
    return &m_oFactory;
  }

private: 

  static FACTORY_TYPE m_oFactory;

};

template<typename T, size_t SIZE>
using TypedPooledObject = PooledObject<TypedFactory<T, SIZE>>;

template<template<typename> class T, size_t SIZE>
using TemplatePooledObject = PooledObject<TemplateFactory<T, SIZE>>;

template<class FACTORY_TYPE>
typename FACTORY_TYPE PooledObject<FACTORY_TYPE>::m_oFactory;

