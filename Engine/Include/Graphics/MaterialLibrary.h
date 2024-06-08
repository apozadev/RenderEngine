#include "Core/Singleton.h"

#include <vector>

#include "Graphics/Material.h"

class Window;

class MaterialLibrary : public Singleton<MaterialLibrary>
{

  struct WindowMatPair
  {
    Window* m_pWindow;
    Material m_oMaterial;
  };

public:

  Material* CreateMaterial(Window* _pWindow) 
  {    
    m_lstMaterials.push_back(WindowMatPair{ _pWindow, std::move(Material(_pWindow)) });
    return &(m_lstMaterials.back().m_oMaterial);
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
      }
    }
  }

private:  

  std::vector<WindowMatPair> m_lstMaterials;
};