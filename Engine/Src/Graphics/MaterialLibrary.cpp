#include "Graphics/MaterialLibrary.h"
#include "Graphics/Pass.h"

#include "Reflection/ReflectionHelper.h"


const Material* MaterialLibrary::LoadMaterial(const char* _sFilename)
{
  owner_ptr<Material> pMaterial = Factory::Create<Material>();
  reflection::ReflectionHelper::Deserialize(_sFilename, pMaterial.get());  
  pMaterial->Setup();
  m_lstMaterials.push_back({ _sFilename, std::move(pMaterial) });
  return m_lstMaterials.back().m_pMaterial.get();
}

const std::string& MaterialLibrary::FindMaterialFilename(const Material* _pMaterial) const
{
  static std::string s_sEmpty("");

  for (const MatEntry& rMatEntry : m_lstMaterials)
  {
    if (rMatEntry.m_pMaterial.get() == _pMaterial)
    {
      return rMatEntry.m_sFilename;
    }
  }

  return s_sEmpty;
}
