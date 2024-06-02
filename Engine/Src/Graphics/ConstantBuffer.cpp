#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/ResourceBindInfo.h"

class ConstantBufferBase::Impl
{
public:
  api::APIConstantBuffer* m_pAPICbuffer;    

  Impl(size_t _uSize)
  {
    m_pAPICbuffer = api::CreateAPIConstantBuffer(_uSize);
  }

  ~Impl()
  {
    api::DestroyAPIConstantBuffer(m_pAPICbuffer);
  }

};

ConstantBufferBase::ConstantBufferBase(size_t _uSize, int _iBinding, PipelineStage _eStage)
  : Resource(_iBinding, _eStage)
{
  m_pImpl = std::make_unique<Impl>(_uSize);  
}

ConstantBufferBase::~ConstantBufferBase()
{
}

void ConstantBufferBase::Update(const void* _pData, size_t _uSize) const
{
  api::UpdateAPIConstantBuffer(m_pImpl->m_pAPICbuffer, _pData, _uSize);  
}

void ConstantBufferBase::Setup(size_t _uSize, ResourceFrequency _eFrequency) const
{  
  ResourceBindInfo oBindInfo{};
  oBindInfo.m_eLevel = _eFrequency;
  oBindInfo.m_eStage = m_eStage;
  oBindInfo.m_iBinding = m_iBinding;
  api::SubStateSetupConstantBuffer(m_pImpl->m_pAPICbuffer, _uSize, oBindInfo);
}

void ConstantBufferBase::Bind() const
{  
  api::BindAPIConstantBuffer(m_pImpl->m_pAPICbuffer);
}
