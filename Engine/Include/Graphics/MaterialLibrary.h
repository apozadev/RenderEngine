#pragma once

#include "Core/Singleton.h"

#include <vector>
#include <string>

#include "Graphics/Material.h"
#include "Memory/Factory.h"

class Window;

typedef uint32_t MaterialId;

class MaterialLibrary : public Singleton<MaterialLibrary>
{ 

public:

  struct MatEntry
  {
    std::string m_sFilename;
    owner_ptr<Material> m_pMaterial;
  };

public:    

  const Material* LoadMaterial(const char* _sFilename);

  const std::vector<MatEntry>& GetMaterials() const { return m_lstMaterials; }

  const std::string& FindMaterialFilename(const Material* _pMaterial) const;

  void Clear()
  {
    m_lstMaterials.clear();
  }

protected:

  using Singleton<MaterialLibrary>::Singleton;

private:  

  std::vector<MatEntry> m_lstMaterials;

  MaterialId m_uNextId = 0u;
};