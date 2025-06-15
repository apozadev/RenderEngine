#include "Graphics/ConstantBuffer.h"
#include "Graphics/ResourceBindInfo.h"
#include "Core/Engine.h"
#include "Graphics/API/GraphicsAPI.h"

void ConstantBufferBase::Configure(size_t _uSize)
{  
  m_pAPICbuffer = api::CreateAPIConstantBuffer(ENGINE_API_WINDOW, _uSize);
}

ConstantBufferBase::~ConstantBufferBase()
{
  api::DestroyAPIConstantBuffer(ENGINE_API_WINDOW, m_pAPICbuffer);
}

void ConstantBufferBase::Update(const void* _pData, size_t _uSize) const
{
  api::UpdateAPIConstantBuffer(ENGINE_API_WINDOW, m_pAPICbuffer, _pData, _uSize);
}

void ConstantBufferBase::Bind() const
{  
  api::BindAPIConstantBuffer(ENGINE_API_WINDOW, m_pAPICbuffer);
}