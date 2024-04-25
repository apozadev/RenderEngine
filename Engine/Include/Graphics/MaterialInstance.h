#pragma once

#include "Graphics/Material.h"

#include <vector>

class Resource;

class MaterialInstance
{
public:

  MaterialInstance(Material* _pMaterial) : m_pMaterial(_pMaterial), m_bSetup(false)
  {}

  ~MaterialInstance();

  template<class T, typename ...Args>
  inline T* AddResource(Args&&... args)
  {     
    T* pResource = new T(std::forward<Args>(args)...);
    m_lstResources.push_back(pResource);
    return pResource;
  }

  Material* GetMaterial() const { return m_pMaterial; }

  void Setup();

  void Bind() const;  

private:

  Material* m_pMaterial;

  std::vector<Resource*> m_lstResources;

  bool m_bSetup;
};