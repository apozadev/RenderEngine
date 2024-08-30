#include "Core/Singleton.h"

#include <vector>

#include "Graphics/Material.h"

class Window;

typedef uint32_t MaterialId;

class MaterialLibrary : public Singleton<MaterialLibrary>
{

  struct WindowMatPair
  {    
    Window* m_pWindow;
    std::unique_ptr<Material> m_pMaterial;
  };

public:

  Material* CreateMaterial(Window* _pWindow, const std::string& _sVSFilename, const std::string& _sPSFilename)
  {    
    m_lstMaterials.push_back(WindowMatPair{ _pWindow, std::make_unique<Material>(_pWindow, _sVSFilename, _sPSFilename) });
    return m_lstMaterials.back().m_pMaterial.get();
  }

  void DestroyWindowMaterials(Window* _pWindow)
  {
    for (int i = 0; i < m_lstMaterials.size(); i++)
    {
      if (m_lstMaterials[i].m_pWindow == _pWindow)
      {
        if (i < m_lstMaterials.size() - 1)
        {
          WindowMatPair oAux(std::move(m_lstMaterials[i]));
          m_lstMaterials[i] = std::move(m_lstMaterials[m_lstMaterials.size() - 1]);
          m_lstMaterials[m_lstMaterials.size() - 1] = std::move(oAux);
        }
        m_lstMaterials.pop_back();
        i--;
      }
    }
  }

private:  

  std::vector<WindowMatPair> m_lstMaterials;

  MaterialId m_uNextId = 0u;
};