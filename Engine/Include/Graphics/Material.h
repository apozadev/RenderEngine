#pragma once

#include <vector>
#include <memory>

class Resource;
class Window;

class Material
{
public:

  Material(Window* _pWindow);
  ~Material();

  template<class T, typename ...Args>
  inline T* AddResource(Args&&... args)
  {    
    SetUsingRenderState();
    T* pResource = new T(std::forward<Args>(args)...);
    m_lstResources.push_back(pResource);
    return pResource;
  }

  void Bind() const;

  void SetUsingRenderState() const;

private:      

  class Impl;
  std::unique_ptr<Impl> m_pImpl;

  std::vector<Resource*> m_lstResources;
};