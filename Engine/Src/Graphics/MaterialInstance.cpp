#include "Graphics/MaterialInstance.h"
#include "Graphics/Material.h"
#include "Graphics/Resource.h"
#include "Graphics/ResourceBindInfo.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Core/Exception.h"

class MaterialInstance::Impl
{
public:

  Impl(Material* _pMaterial)
    : m_pMaterial(_pMaterial)
  {    
    m_pMaterial->SetUsing();
    m_pSubState = api::CreateAPIRenderSubState();
    //api::SetUsingAPIRenderState(nullptr);
  }

  ~Impl()
  {
    api::DestroyRenderSubState(m_pSubState);
  }

  Material* m_pMaterial;

  api::APIRenderSubState* m_pSubState;
};

MaterialInstance::MaterialInstance(Material* _pMaterial) : m_bSetup(false)
{
  m_pImpl = std::make_unique<Impl>(_pMaterial);
}

MaterialInstance::MaterialInstance(MaterialInstance&& _rMatInstance) : m_pImpl(std::move(_rMatInstance.m_pImpl))
{
}

MaterialInstance::~MaterialInstance()
{
  for (Resource* pResource : m_lstResources)
  {
    delete pResource;
  }
}

void MaterialInstance::Setup()
{  

  api::BeginSubStateSetup(m_pImpl->m_pSubState);

  for (const Resource* pResource : m_lstResources)
  {        
    pResource->Setup(ResourceFrequency::MATERIAL_INSTANCE);
  }

  api::EndSubStateSetup();
  
  m_bSetup = true;
}

Material* MaterialInstance::GetMaterial() const
{
  return m_pImpl->m_pMaterial;
}

void MaterialInstance::Bind() const
{  
  if (!m_bSetup)
  {
    THROW_GENERIC_EXCEPTION("Material Instance was not set up")
  }  

  api::BindAPIRenderSubState(m_pImpl->m_pSubState);

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}