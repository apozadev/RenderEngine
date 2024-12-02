#pragma once

#include "Graphics/API/GraphicsAPI.h"
#include "Core/BaseObject.h"

class ConstantBufferBase : public BaseObject
{

public:
  
  virtual ~ConstantBufferBase();  

  virtual void SetupRenderSubState(ResourceFrequency _eFrequency) const = 0;

  void Bind() const;

protected:  

  void Configure(size_t _uSize, int _iBinding, PipelineStage _eStage);

  void Update(const void* _pData, size_t _uSize) const;

  void SetupRenderSubState(size_t _uSize, ResourceFrequency _eFrequency) const;

private:

  api::APIConstantBuffer* m_pAPICbuffer;

  int m_iBinding;

  PipelineStage m_eStage;
};

template <typename T>
class ConstantBuffer : public ConstantBufferBase
{  
public:      

  void Configure(int _iBinding, PipelineStage _eStage)
  {
    ConstantBufferBase::Configure(sizeof(T), _iBinding, _eStage);
  }

  void SetupRenderSubState(ResourceFrequency _eFrequency) const override
  {
    ConstantBufferBase::SetupRenderSubState(sizeof(T), _eFrequency);
  }

  void Update() const
  {
    ConstantBufferBase::Update(&m_oData, sizeof(T));
  }

  T* GetData()
  {
    return &m_oData;
  }

  void SetData(T* _pData)
  {
    m_oData = *_pData;
  }    

protected:

  T m_oData;
};