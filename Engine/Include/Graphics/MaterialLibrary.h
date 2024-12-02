#include "Core/Singleton.h"

#include <vector>

#include "Graphics/Material.h"
#include "Memory/Factory.h"

class Window;

typedef uint32_t MaterialId;

class MaterialLibrary : public Singleton<MaterialLibrary>
{ 
public:

  Material* CreateMaterial()
  {    
    owner_ptr<Material> pMaterial = Factory::Create<Material>();
    m_lstMaterials.push_back(std::move(pMaterial));
    return m_lstMaterials.back().get();
  }

  void Clear()
  {
    m_lstMaterials.clear();
  }

private:  

  std::vector<owner_ptr<Material>> m_lstMaterials;

  MaterialId m_uNextId = 0u;
};