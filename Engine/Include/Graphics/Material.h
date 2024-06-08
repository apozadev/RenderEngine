#pragma once

#include <vector>
#include <memory>

class Resource;
class Window;

class Material
{
public:

  Material(Window* _pWindow);
  Material(Material&& rMaterial);
  ~Material();

  template<class T, typename ...Args>
  inline T* AddResource(Args&&... args)
  {        
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