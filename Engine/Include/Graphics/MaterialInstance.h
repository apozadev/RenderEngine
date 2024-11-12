#pragma once

#include <memory>
#include <vector>

#include "Graphics/Window.h"
#include "Graphics/Material.h"
#include "Graphics/API/GraphicsAPI.h"

class Resource;

class MaterialInstance
{
public:
  
  MaterialInstance(Material* _pMaterial);
  MaterialInstance(MaterialInstance&& _rMatInstance);
  ~MaterialInstance();

  template<class T, typename ...Args>
  inline T* AddResource(Args&&... args)
  {         
    T* pResource = new T(std::forward<Args>(args)...);
    m_lstResources.push_back(pResource);
    return pResource;
  }

  Material* GetMaterial() const { return m_pMaterial; }

  uint16_t GetId() const { return m_uId; }

  void Setup();

  void Bind() const;    

private:  

  std::vector<Resource*> m_lstResources;

  bool m_bSetup;

  uint16_t m_uId;

  Material* m_pMaterial;

  api::APIRenderSubState* m_pSubState;

};