#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"

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

ConstantBufferBase::ConstantBufferBase(size_t _uSize)
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


void ConstantBufferBase::Bind() const
{  
  api::BindAPIConstantBuffer(m_pImpl->m_pAPICbuffer);
}
