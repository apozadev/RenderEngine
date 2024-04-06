#include "Graphics/Material.h"

#include "Graphics/Renderer.h"

#include "Graphics/Resource.h"

#include "Graphics/API/GraphicsAPI.h"

#include "Graphics/Window.h"

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
}

void Material::Bind() const
{
  api::BindAPIRenderState(m_pImpl->m_pAPIRenderState);

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}

void Material::SetUsingRenderState() const
{
  api::SetUsingRenderState(m_pImpl->m_pAPIRenderState);
}
