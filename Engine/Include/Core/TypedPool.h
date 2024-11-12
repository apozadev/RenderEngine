#pragma once

#include <cstdint>
#include <utility>

#include "Core/Exception.h"

template<typename T, size_t SIZE>
class TypedPool
{
private:

  struct Entry
  {
    T m_oElem;
    bool m_bUsed = false;    
  };

public:

  void Initialize()
  {
    m_pData = reinterpret_cast<Entry*>(malloc(sizeof(T) * SIZE));
    memset(m_pData, 0, sizeof(T) * SIZE);
    m_uCurrIdx = 0u;
  }

  bool IsInitialized()
  {
    return m_pData != nullptr;
  }

  template <typename ...Args>
  T* PullElement(Args&&... args)
  {        
    T* pElement = PullElementNoConstruct();
    new (pElement) T(std::forward<Args>(args)...);
    return pElement;
  }

  T* PullElementNoConstruct()
  {
    for (size_t i = 0u; i < SIZE; i++)
    {
      size_t uIdx = (i + m_uCurrIdx) % (SIZE - 1u);
      Entry& rEntry = m_pData[uIdx];
      if (!rEntry.m_bUsed)
      {
        m_uCurrIdx = uIdx + 1u;
        rEntry.m_bUsed = true;        
        return &rEntry.m_oElem;
      }
    }

    THROW_GENERIC_EXCEPTION("Pool not big enough")

      return nullptr;
  }

  void ReturnElement(T* _pElement)
  {
    size_t uIdx = (reinterpret_cast<std::uintptr_t>(_pElement) - reinterpret_cast<std::uintptr_t>(m_pData)) / sizeof(Entry);
    if (uIdx > SIZE)
    {
      THROW_GENERIC_EXCEPTION("Tried to return an element to the wrong pool")
    }    

    m_pData[uIdx].m_bUsed = false;

    m_pData[uIdx].m_oElem.~T();
  }

  void Clear()
  {
    delete m_pData;
    m_uCurrIdx = 0u;
  }

private:
  
  Entry* m_pData = nullptr;

  size_t m_uCurrIdx;  
};
