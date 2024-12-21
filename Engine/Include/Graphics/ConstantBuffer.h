#pragma once

#include <string>

#include "Graphics/API/GraphicsAPI.h"
#include "Core/BaseObject.h"

class ConstantBufferBase : public BaseObject
{

public:
  
  virtual ~ConstantBufferBase();  

  virtual void SetupRenderSubState(const std::string& _sName, PipelineStage _eStage, ResourceFrequency _eFrequency) const = 0;

  void Bind() const;

protected:  

  void Configure(size_t _uSize);

  void Update(const void* _pData, size_t _uSize) const;

  void SetupRenderSubState(size_t _uSize, const std::string& _sName, PipelineStage _eStage, ResourceFrequency _eFrequency) const;

private:

  api::APIConstantBuffer* m_pAPICbuffer;
};

template <typename T>
class ConstantBuffer : public ConstantBufferBase
{  
public:      

  void Configure()
  {
    ConstantBufferBase::Configure(sizeof(T));
  }

  void SetupRenderSubState(const std::string& _sName, PipelineStage _eStage, ResourceFrequency _eFrequency) const override
  {
    ConstantBufferBase::SetupRenderSubState(sizeof(T), _sName, _eStage, _eFrequency);
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