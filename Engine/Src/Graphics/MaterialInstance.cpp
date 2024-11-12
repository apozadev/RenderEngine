#include "Graphics/MaterialInstance.h"
#include "Graphics/Resource.h"
#include "Graphics/ResourceBindInfo.h"
#include "Core/Exception.h"

namespace matinstance_internal
{
  static uint16_t s_uNextId = 0u;
}

MaterialInstance::MaterialInstance(Material* _pMaterial) 
  : m_bSetup(false)
  , m_pMaterial(_pMaterial)
{
  m_uId = matinstance_internal::s_uNextId++;
  m_pSubState = api::CreateAPIRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);
}

MaterialInstance::MaterialInstance(MaterialInstance&& _rMatInstance)   
  : m_lstResources(std::move(_rMatInstance.m_lstResources))
  , m_bSetup(std::move(_rMatInstance.m_bSetup))
  , m_uId(std::move(_rMatInstance.m_uId))
  , m_pMaterial(std::move(_rMatInstance.m_pMaterial))  
  , m_pSubState(std::move(_rMatInstance.m_pSubState))
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
  api::BeginSubStateSetup(m_pSubState);

  for (const Resource* pResource : m_lstResources)
  {        
    pResource->SetupRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);
  }

  api::EndSubStateSetup(ResourceFrequency::MATERIAL_INSTANCE);
  
  m_bSetup = true;
}

void MaterialInstance::Bind() const
{  
  if (!m_bSetup)
  {
    THROW_GENERIC_EXCEPTION("Material Instance was not set up")
  }  

  api::BindAPIRenderSubState(m_pSubState, ResourceFrequency::MATERIAL_INSTANCE);

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}
