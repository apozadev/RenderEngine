#include "Graphics/Material.h"

#include "Graphics/Resource.h"

#include "Graphics/API/GraphicsAPI.h"

#include "Graphics/Window.h"

#include "Core/Exception.h"

class Material::Impl
{
public:
  api::APIRenderState* m_pAPIRenderState;

  Impl(Window* _pWindow)
  {
    _pWindow->SetUsing();
    m_pAPIRenderState = api::CreateAPIRenderState();
  }

  ~Impl()
  {
    api::DestroyAPIRenderState(m_pAPIRenderState);
  }
};

Material::Material(Window* _pWindow)
{
  m_pImpl = std::make_unique<Impl>(_pWindow);    
}

Material::~Material()
{
  for (Resource* pResource : m_lstResources)
  {
    delete pResource;
  }
}

void Material::Setup() const
{
  api::BeginRenderStateSetup(m_pImpl->m_pAPIRenderState);

  for (Resource* pResource : m_lstResources)
  {
    pResource->Setup(ResourceFrequency::MATERIAL);
  }

  api::EndRenderStateSetup();
}

void Material::Bind() const
{

  api::BindAPIRenderState(m_pImpl->m_pAPIRenderState);

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}

void Material::SetUsing() const
{
  api::SetUsingAPIRenderState(m_pImpl->m_pAPIRenderState);
}

