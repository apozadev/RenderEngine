#pragma once

#include <cstdint>

#include "Core/Exception.h"

template<typename T, size_t SIZE>
class TypedContiguousPool
{
public:

  void Initialize()
  {
    m_pData = new T[SIZE];
    m_pArraySizes = new size_t[SIZE];   
    memset(m_pArraySizes, 0, SIZE * sizeof(size_t));
    m_uCurrIdx = 0u;
  }

  T* PullElements(size_t _uCount)
  {
    if (_uCount > 0u)
    {
      size_t uCandidateIdx = SIZE;

      for (size_t i = 0u; i < SIZE;)
      {
        size_t uIdx = (i + m_uCurrIdx) % SIZE;

        if (uIdx < uCandidateIdx)
        {
          uCandidateIdx = SIZE;
        }

        if (uCandidateIdx >= SIZE)
        {
          if (m_pArraySizes[uIdx] == 0u)
          {
            uCandidateIdx = uIdx;
          }
        }

        if (uCandidateIdx < SIZE
          && m_pArraySizes[uIdx] == 0u
          && uIdx - uCandidateIdx >= _uCount - 1u)
        {
          m_uCurrIdx = uIdx + 1;
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
    }

    return nullptr;
  }

  void ReturnElements(T* _pElement)
  {
    size_t uIdx = (reinterpret_cast<std::uintptr_t>(_pElement) - reinterpret_cast<std::uintptr_t>(m_pData)) / sizeof(T);
    if (uIdx > SIZE)
    {
      THROW_GENERIC_EXCEPTION("Tried to return an element to the wrong pool")
    }

    if (m_pArraySizes[uIdx] == 0u)
    {
      THROW_GENERIC_EXCEPTION("The elements have been already freed OR this is not the first element")
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
    m_uCurrIdx = 0u;
  }

private:

  T* m_pData;

  size_t* m_pArraySizes;

  size_t m_uCurrIdx;
};
