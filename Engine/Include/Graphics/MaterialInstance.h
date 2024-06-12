#pragma once

#include <memory>
#include <vector>

#include "Graphics/Window.h"

class Resource;

class Material;

class MaterialInstance
{
public:
  
  MaterialInstance(Material* _pMaterial);
  MaterialInstance(MaterialInstance&& _rMatInstance);
  ~MaterialInstance();

  template<class T, typename ...Args>
  inline T* AddResource(Window* _pWindow, Args&&... args)
  {     
    _pWindow->SetUsing();
    T* pResource = new T(std::forward<Args>(args)...);
    m_lstResources.push_back(pResource);
    return pResource;
  }

  Material* GetMaterial() const;

  void Setup();

  void Bind() const;  

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;  

  std::vector<Resource*> m_lstResources;

  bool m_bSetup;
};