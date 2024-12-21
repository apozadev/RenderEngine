#include "Graphics/ConstantBuffer.h"
#include "Graphics/ResourceBindInfo.h"

void ConstantBufferBase::Configure(size_t _uSize)
{  
  m_pAPICbuffer = api::CreateAPIConstantBuffer(_uSize);  
}

ConstantBufferBase::~ConstantBufferBase()
{
  api::DestroyAPIConstantBuffer(m_pAPICbuffer);
}

void ConstantBufferBase::Update(const void* _pData, size_t _uSize) const
{
  api::UpdateAPIConstantBuffer(m_pAPICbuffer, _pData, _uSize);  
}

void ConstantBufferBase::SetupRenderSubState(size_t _uSize, const std::string& _sName, PipelineStage _eStage, ResourceFrequency _eFrequency) const
{  
  ResourceBindInfo oBindInfo{};
  oBindInfo.m_eLevel = _eFrequency;
  oBindInfo.m_eStage = _eStage;
  oBindInfo.m_sName = _sName;
  api::SubStateSetupConstantBuffer(m_pAPICbuffer, _uSize, oBindInfo);
}

void ConstantBufferBase::Bind() const
{  
  api::BindAPIConstantBuffer(m_pAPICbuffer);
}