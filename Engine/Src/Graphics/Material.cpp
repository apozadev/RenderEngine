#include "Graphics/Material.h"

#include "Graphics/Resource.h"

#include "Graphics/API/GraphicsAPI.h"

#include "Graphics/Window.h"

#include "Graphics/Mesh.h"

#include "Graphics/RenderStateInfo.h"

#include "Core/Exception.h"

class Material::Impl
{
public:
  api::APIRenderState* m_pAPIRenderState;
  std::vector<Resource*> m_lstResources;

  Impl(Window* _pWindow)
  {
    _pWindow->SetUsing();

    RenderStateInfo oInfo{};
    oInfo.m_uMeshConstantSize = sizeof(MeshConstant);

    m_pAPIRenderState = api::CreateAPIRenderState(oInfo);
  }

  ~Impl()
  {
    for (Resource* pResource : m_lstResources)
    {
      delete pResource;
    }

    api::DestroyAPIRenderState(m_pAPIRenderState);
  }
};

Material::Material(Window* _pWindow)
{
  m_pImpl = std::make_unique<Impl>(_pWindow);    
}

Material::Material(Material&& rMaterial)
  : m_pImpl(std::move(rMaterial.m_pImpl))
{
}

Material::~Material()
{
}

void Material::Setup() const
{
  api::BeginRenderStateSetup(m_pImpl->m_pAPIRenderState);

  for (Resource* pResource : m_pImpl->m_lstResources)
  {
    pResource->Setup(ResourceFrequency::MATERIAL);
  }

  api::EndRenderStateSetup();
}

void Material::Bind() const
{

  api::BindAPIRenderState(m_pImpl->m_pAPIRenderState);

  for (const Resource* pResource : m_pImpl->m_lstResources)
  {
    pResource->Bind();
  }
}

void Material::SetUsing() const
{
  api::SetUsingAPIRenderState(m_pImpl->m_pAPIRenderState);
}

void Material::AddResourceInternal(Resource* _pResource)
{
  m_pImpl->m_lstResources.push_back(_pResource);
}

Material& Material::operator=(Material&& _rMaterial)
{  
  m_pImpl = std::move(_rMaterial.m_pImpl);
  return *this;
}
