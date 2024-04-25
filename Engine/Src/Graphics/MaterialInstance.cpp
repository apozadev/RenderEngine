#include "Graphics/MaterialInstance.h"
#include "Graphics/Material.h"
#include "Graphics/Resource.h"
#include "Core/Exception.h"

MaterialInstance::~MaterialInstance()
{
  for (Resource* pResource : m_lstResources)
  {
    delete pResource;
  }
}

void MaterialInstance::Setup()
{
  m_pMaterial->BeginInstanceSetup(Material::SetupKey());

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Setup();
  }

  m_pMaterial->EndInstanceSetup(Material::SetupKey());

  m_bSetup = true;
}

void MaterialInstance::Bind() const
{  
  if (!m_bSetup)
  {
    THROW_GENERIC_EXCEPTION("Material Instance was not set up")
  }  

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}