#pragma once

#include <memory>
#include <vector>

#include "Memory/GenericPool.h"
#include "Core/Exception.h"

class Factory
{
private:

  template<typename T>
  friend struct PoolObjectDeleter;

public:

  static void Init(size_t globalSize, size_t transientSize)
  {
    m_globalPool.Initialize(globalSize);
    m_transientPool.Initialize(transientSize);
    m_modeStack.push_back(true);
  }

  template<class T, typename ...Args>
  static inline std::unique_ptr<T, PoolObjectDeleter<T>> Create(Args&&... args)
  {

    T* pObj = nullptr;

    if (IsCurrentModeGlobal())
    {
      pObj = (T*)m_globalPool.Allocate(sizeof(T), alignof(T));
    }
    else
    {
      pObj = (T*)m_transientPool.Allocate(sizeof(T), alignof(T));
    }

    if (pObj != nullptr)
    {
      new (pObj) T(std::forward<Args>(args)...);
    }

    return std::unique_ptr<T, PoolObjectDeleter<T>>(pObj, PoolObjectDeleter<T>());
  }

  static void SetGlobalMode(bool enabled)
  {
    m_modeStack.clear();
    m_modeStack.push_back(enabled);
  }

  static void PushGlobalMode(bool enabled)
  {
    m_modeStack.push_back(enabled);
  }

  static void PopGlobalMode()
  {
    if (m_modeStack.size() > 1)
    {
      m_modeStack.pop_back();
    }
  }

  static bool IsCurrentModeGlobal()
  {
    return m_modeStack.empty() ? true : m_modeStack[m_modeStack.size() - 1];
  }

private:

  template<class T>
  static inline void Destroy(T* pObj)
  {
    if (pObj == nullptr)
    {
      return;
    }

    pObj->~T();

    if (!m_transientPool.Deallocate(pObj) && !m_globalPool.Deallocate(pObj))
    {
      THROW_GENERIC_EXCEPTION("[MEMORY] the object you tried to deallocate is not in any pool")
    }
  }

  inline static GenericPool m_globalPool;
  inline static GenericPool m_transientPool;
  inline static std::vector<bool> m_modeStack;
};

