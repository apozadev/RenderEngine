#include "Core/Singleton.h"

#include <vector>

#include "Graphics/Material.h"
#include "Memory/Factory.h"

class Window;

typedef uint32_t MaterialId;

class MaterialLibrary : public Singleton<MaterialLibrary>
{ 
public:  

  Material* CreateMaterial();

  const Material* LoadMaterial(const char* _sFilename);

  void Clear()
  {
    m_lstMaterials.clear();
  }

protected:

  using Singleton<MaterialLibrary>::Singleton;

private:  

  std::vector<owner_ptr<Material>> m_lstMaterials;

  MaterialId m_uNextId = 0u;
};