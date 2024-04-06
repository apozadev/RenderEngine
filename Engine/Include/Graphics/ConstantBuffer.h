#pragma once

#include <memory>

class ConstantBufferBase
{
public:

  ConstantBufferBase(size_t _uSize);
  ~ConstantBufferBase();

  void Update(const void* _pData, size_t _uSize) const;

  void Bind() const;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};

template <typename T>
class ConstantBuffer :public ConstantBufferBase
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

  void SetData(T* _pData)
  {
    m_oData = *_pData
  }

  void Update() const
  {
    ConstantBufferBase::Update(&m_oData, sizeof(T));
  }

private:

  T m_oData;
};