#include "Graphics/MaterialLibrary.h"
#include "Graphics/Pass.h"

#include "Reflection/ReflectionHelper.h"

Material* MaterialLibrary::CreateMaterial()
{  
  m_lstMaterials.push_back(Factory::Create<Material>());
  return m_lstMaterials.back().get();
}

const Material* MaterialLibrary::LoadMaterial(const char* _sFilename)
{
  Material* pMaterial = CreateMaterial();
  reflection::ReflectionHelper::Deserialize(_sFilename, pMaterial);  
  pMaterial->Setup();
  return pMaterial;
}
