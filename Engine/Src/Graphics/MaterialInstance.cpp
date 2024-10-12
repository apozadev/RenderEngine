#include "Graphics/MaterialInstance.h"
#include "Graphics/Material.h"
#include "Graphics/Resource.h"
#include "Graphics/ResourceBindInfo.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Core/Exception.h"

namespace matinstance_internal
{
  static uint16_t s_uNextId = 0u;
}

class MaterialInstance::Impl
{
public:

  Impl(Material* _pMaterial)
    : m_pMaterial(_pMaterial)
  {        
    m_pSubState = api::CreateAPIRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);    
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
  m_uId = matinstance_internal::s_uNextId++;
  m_pImpl = std::make_unique<Impl>(_pMaterial);
}

MaterialInstance::MaterialInstance(MaterialInstance&& _rMatInstance) 
  : m_pImpl(std::move(_rMatInstance.m_pImpl))
  , m_lstResources(std::move(_rMatInstance.m_lstResources))
  , m_bSetup(std::move(_rMatInstance.m_bSetup))
  , m_uId(std::move(_rMatInstance.m_uId))
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
    pResource->SetupRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);
  }

  api::EndSubStateSetup(ResourceFrequency::MATERIAL_INSTANCE);
  
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

  api::BindAPIRenderSubState(m_pImpl->m_pSubState, ResourceFrequency::MATERIAL_INSTANCE);

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}
