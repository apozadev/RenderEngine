#include "Core/Singleton.h"

#include <vector>

#include "Graphics/Material.h"

class Window;

typedef uint32_t MaterialId;

class MaterialLibrary : public Singleton<MaterialLibrary>
{ 
public:

  Material* CreateMaterial(std::vector<pooled_ptr<Pass>>&& _lstPasses)
  {    
    m_lstMaterials.push_back(std::move(std::make_unique<Material>(std::move(_lstPasses))));

    return m_lstMaterials.back().get();
  }

  void Clear()
  {
    m_lstMaterials.clear();
  }

private:  

  std::vector<std::unique_ptr<Material>> m_lstMaterials;

  MaterialId m_uNextId = 0u;
};