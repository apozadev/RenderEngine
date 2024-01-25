#pragma once

#include <memory>

namespace core
{
  template <class T>
  class Singleton
  {
  public:
    static T* GetInstance()
    {
      static T* m_instance;
      if (!m_instance)
      {
        m_instance = new T();
      }
      return m_instance;
    }
  };
}