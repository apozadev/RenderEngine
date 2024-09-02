#pragma once

#include <vector>
#include <memory>

#include "Graphics/Window.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/RenderStateInfo.h"

class Resource;

class Material
{
public:

  Material(Window* _pWindow
    , const std::string& _sVSFilename
    , const std::string& _sPSFilename
    , bool _bBlendEnabled
    , BlendOp _eBlendOp
    , BlendFactor _eSrcBlendFactor
    , BlendFactor _eDstBlendFactor
    , bool _bDepthWrite
    , bool _bDepthRead);

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

  const RenderStateInfo& GetRenderStateInfo() { return m_oInfo; }

  Material& operator=(Material&& _rMaterial);

private:      

  void AddResourceInternal(Resource* _pResource);

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;  

  RenderStateInfo m_oInfo;
};