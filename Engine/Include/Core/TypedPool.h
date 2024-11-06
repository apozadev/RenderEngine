#pragma once

#include <cstdint>

#include "Core/Exception.h"

template<typename T>
class TypedPool
{
private:

  struct Entry
  {
    T m_oElem;
    bool m_bUsed = false;    
  };

public:

  void Initialize(size_t _uSize)
  {
    m_pData = new Entry[_uSize];
    m_uSize = _uSize;
    m_uCurrIdx = 0u;
  }

  T* PullElement()
  {        
    for(size_t i = 0u; i < m_uSize; i++)
    {
      size_t uIdx = (i + m_uCurrIdx) % (m_uSize-1u);
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
    if (uIdx > m_uSize)
    {
      THROW_GENERIC_EXCEPTION("Tried to return an element to the wrong pool")
    }    

    m_pData[uIdx].m_bUsed = false;

    m_pData[uIdx].m_oElem.~T();
  }

  void Clear()
  {
    delete [] m_pData;
    m_uSize = 0u;
    m_uCurrIdx = 0u;
  }

private:
  
  Entry* m_pData;

  size_t m_uSize;

  size_t m_uCurrIdx;  
};
