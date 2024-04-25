#pragma once

#include <vector>
#include <memory>

class Resource;
class Window;

class Material
{
public:

  class SetupKey
  {  
  private:
    friend class MaterialInstance;
    SetupKey() {};
  };

public:

  Material(Window* _pWindow);
  ~Material();

  template<class T, typename ...Args>
  inline T* AddResource(Args&&... args)
  {        
    T* pResource = new T(std::forward<Args>(args)...);
    m_lstResources.push_back(pResource);
    return pResource;
  }

  void BeginInstanceSetup(SetupKey&&) const;  
  void EndInstanceSetup(SetupKey&&) const;

  void Bind() const;    

private:      

  class Impl;
  std::unique_ptr<Impl> m_pImpl;

  std::vector<Resource*> m_lstResources;
};