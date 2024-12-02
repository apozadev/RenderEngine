#include "Graphics/ConstantBuffer.h"
#include "Graphics/ResourceBindInfo.h"

void ConstantBufferBase::Configure(size_t _uSize, int _iBinding, PipelineStage _eStage)  
{  
  m_pAPICbuffer = api::CreateAPIConstantBuffer(_uSize);
  m_iBinding = _iBinding;
  m_eStage = _eStage;
}

ConstantBufferBase::~ConstantBufferBase()
{
  api::DestroyAPIConstantBuffer(m_pAPICbuffer);
}

void ConstantBufferBase::Update(const void* _pData, size_t _uSize) const
{
  api::UpdateAPIConstantBuffer(m_pAPICbuffer, _pData, _uSize);  
}

void ConstantBufferBase::SetupRenderSubState(size_t _uSize, ResourceFrequency _eFrequency) const
{  
  ResourceBindInfo oBindInfo{};
  oBindInfo.m_eLevel = _eFrequency;
  oBindInfo.m_eStage = m_eStage;
  oBindInfo.m_iBinding = m_iBinding;
  api::SubStateSetupConstantBuffer(m_pAPICbuffer, _uSize, oBindInfo);
}

void ConstantBufferBase::Bind() const
{  
  api::BindAPIConstantBuffer(m_pAPICbuffer);
}