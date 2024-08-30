#pragma once

#include <vector>
#include <memory>

#include "Graphics/Window.h"

class Resource;

class Material
{
public:

  Material(Window* _pWindow, const std::string& _sPSFilename, const std::string& _sVSFilename);
  Material(Material&& rMaterial);
  ~Material();

  template<class T, typename ...Args>
  inline T* AddResource(Window* _pWindow, Args&&... args)
  {        
    _pWindow->SetUsing();
    T* pResource = new T(std::forward<Args>(args)...);
    AddResourceInternal(pResource);
    return pResource;
  }  

  void Setup() const;

  void Bind() const;   

  void SetUsing() const;

  Material& operator=(Material&& _rMaterial);

private:      

  void AddResourceInternal(Resource* _pResource);

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;  
};