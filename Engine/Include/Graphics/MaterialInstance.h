#pragma once

#include "Graphics/Material.h"

#include <vector>

class Resource;

class MaterialInstance
{
public:

  MaterialInstance(Material* _pMaterial) : m_pMaterial(_pMaterial)
  {}

  template<class T, typename ...Args>
  inline T* AddResource(Args&&... args)
  { 
    m_pMaterial->SetUsingRenderState();
    T* pResource = new T(std::forward<Args>(args)...);
    m_lstResources.push_back(pResource);
    return pResource;
  }

  Material* GetMaterial() const { return m_pMaterial; }

  void Bind() const;

private:

  Material* m_pMaterial;

  std::vector<Resource*> m_lstResources;
};