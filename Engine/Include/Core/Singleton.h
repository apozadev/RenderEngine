#pragma once

#include <memory>

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

protected:

  Singleton() = default;
  Singleton(Singleton&) = delete;
  Singleton(Singleton&&) = delete;

  const Singleton& operator=(const Singleton&) = delete;

};
