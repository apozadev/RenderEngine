#pragma once

#include "Graphics/Resource.h"

#include <memory>

class ConstantBufferBase : public Resource
{
protected:

  ConstantBufferBase(size_t _uSize, int _iBinding, PipelineStage _eStage);
  virtual ~ConstantBufferBase();

  void Update(const void* _pData, size_t _uSize) const;

  void Setup(size_t _uSize, ResourceFrequency _eFrequency) const;

public:

  void Bind() const override;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};

template <typename T>
class ConstantBuffer :public ConstantBufferBase
{  
public:

  ConstantBuffer(int _iBinding, PipelineStage _eStage) : ConstantBufferBase(sizeof(T), _iBinding, _eStage)
  {}

  T* GetData()
  {
    return &m_oData;
  }

  void SetData(T* _pData)
  {
    m_oData = *_pData;
  }  

  void Setup(ResourceFrequency _eFrequency) const override
  { 
    ConstantBufferBase::Setup(sizeof(T), _eFrequency);
  }

  void Update() const
  {
    ConstantBufferBase::Update(&m_oData, sizeof(T));
  }

private:

  T m_oData;
};