#include "Graphics/MaterialInstance.h"
#include "Graphics/Material.h"
#include "Graphics/Resource.h"

void MaterialInstance::Bind() const
{
  //m_pMaterial->Bind();

  for (const Resource* pResource : m_lstResources)
  {
    pResource->Bind();
  }
}