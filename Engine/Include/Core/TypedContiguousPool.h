#pragma once

#include <cstdint>

#include "Core/Exception.h"

template<typename T>
class TypedContiguousPool
{
public:

  void Initialize(size_t _uSize)
  {
    m_pData = new T[_uSize];
    m_pArraySizes = new size_t[_uSize];   
    memset(m_pArraySizes, 0, _uSize * sizeof(size_t));
    m_uSize = _uSize;
    m_uCurrIdx = 0u;
  }

  T* PullElements(size_t _uCount)
  {

    size_t uCandidateIdx = m_uSize;

    for (size_t i = 0u; i < m_uSize;)
    {
      size_t uIdx = (i + m_uCurrIdx) % m_uSize;    

      if (uIdx < uCandidateIdx)
      {
        uCandidateIdx = m_uSize;
      }

      if (uCandidateIdx >= m_uSize)
      {
        if (m_pArraySizes[uIdx] == 0u)
        {
          uCandidateIdx = uIdx;
        }
      }
      
      if(uCandidateIdx < m_uSize
        && m_pArraySizes[uIdx] == 0u
        && uIdx - uCandidateIdx >= _uCount-1u)
      {
        m_uCurrIdx = uIdx+1;
        m_pArraySizes[uCandidateIdx] = _uCount;
        return &m_pData[uCandidateIdx];
      }

      if (m_pArraySizes[uIdx] == 0)
      {
        i++;
      }
      else
      {
        i += m_pArraySizes[uIdx];
      } 
    }

    THROW_GENERIC_EXCEPTION("Pool not big enough")

      return nullptr;
  }

  void ReturnElements(T* _pElement)
  {
    size_t uIdx = (reinterpret_cast<std::uintptr_t>(_pElement) - reinterpret_cast<std::uintptr_t>(m_pData)) / sizeof(T);
    if (uIdx > m_uSize)
    {
      THROW_GENERIC_EXCEPTION("Tried to return an element to the wrong pool")
    }

    if (m_pArraySizes[uIdx] == 0u)
    {
      THROW_GENERIC_EXCEPTION("The retured must be the first of the array")
    }

    for (size_t i = uIdx; i < uIdx + m_pArraySizes[uIdx]; i++)
    {
      m_pData[i].~T();
    }

    m_pArraySizes[uIdx] = 0u;
    
  }

  void Clear()
  {
    delete[] m_pData;
    delete[] m_pArraySizes;
    m_uSize = 0u;
    m_uCurrIdx = 0u;
  }

private:

  T* m_pData;

  size_t* m_pArraySizes;

  size_t m_uSize;

  size_t m_uCurrIdx;
};
