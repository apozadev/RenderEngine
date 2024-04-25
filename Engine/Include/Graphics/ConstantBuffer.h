#pragma once

#include "Graphics/Resource.h"

#include <memory>

class ConstantBufferBase : public Resource
{
public:

  ConstantBufferBase(size_t _uSize);
  ~ConstantBufferBase();

  void Update(const void* _pData, size_t _uSize) const;

  void Setup(size_t _uSize) const;

  void Bind() const override;

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

  virtual ~ConstantBuffer()
  {}

  T* GetData()
  {
    return &m_oData;
  }

  void SetData(T* _pData)
  {
    m_oData = *_pData
  }  

  void Setup() const override 
  { 
    ConstantBufferBase::Setup(sizeof(T));
  }

  void Update() const
  {
    ConstantBufferBase::Update(&m_oData, sizeof(T));
  }

private:

  T m_oData;
};