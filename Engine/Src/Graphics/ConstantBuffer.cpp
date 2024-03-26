#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"

class ConstantBufferBase::Impl
{
public:
  api::APIConstantBuffer* m_pAPICbuffer;  
};

ConstantBufferBase::ConstantBufferBase(size_t _uSize)
{
  m_pImpl = new Impl();
  m_pImpl->m_pAPICbuffer = api::CreateAPIConstantBuffer(_uSize);
}

ConstantBufferBase::~ConstantBufferBase()
{
  api::DestroyAPIConstantBuffer(m_pImpl->m_pAPICbuffer);
  delete m_pImpl;
}

void ConstantBufferBase::Bind(const void* _pData, size_t _uSize) const
{
  api::UpdateAPIConstantBuffer(m_pImpl->m_pAPICbuffer, _pData, _uSize);
  api::BindAPIConstantBuffer(m_pImpl->m_pAPICbuffer);
}
