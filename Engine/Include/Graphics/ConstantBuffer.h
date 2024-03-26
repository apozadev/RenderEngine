#pragma once

class ConstantBufferBase
{
public:

  ConstantBufferBase(size_t _uSize);
  ~ConstantBufferBase();

  void Bind(const void* _pData, size_t _uSize) const;

private:

  class Impl;
  Impl* m_pImpl;
};

template <typename T>
class ConstantBuffer : ConstantBufferBase
{  
public:

  ConstantBuffer() : ConstantBufferBase(sizeof(T))
  {}

  ~ConstantBuffer()
  {}

  T* GetData()
  {
    return &m_oData;
  }

  void Bind() const
  {
    ConstantBufferBase::Bind(&m_oData, sizeof(T));
  }

private:

  T m_oData;
};