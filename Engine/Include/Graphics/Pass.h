#pragma once

#include <memory>

#include "Graphics/Window.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/RenderStateInfo.h"

class Resource;

class Pass
{
public:

  Pass(Window* _pWindow
    , const std::string& _sVSFilename
    , const std::string& _sPSFilename
    , bool _bBlendEnabled
    , BlendOp _eBlendOp
    , BlendFactor _eSrcBlendFactor
    , BlendFactor _eDstBlendFactor
    , bool _bDepthWrite
    , bool _bDepthRead
    , const std::string& _sPipelineId
    , int _uStepIdx
    , uint16_t _uLayer);

  Pass(Pass&& rPass);
  ~Pass();

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

  const RenderStateInfo& GetRenderStateInfo() const { return m_oInfo; }

  uint16_t GetId() const { return m_uId; }

  uint16_t GetLayer() const { return m_uLayer; }

  const std::string& GetRenderPipelineId() const;

  int GetRenderStepIdx() const;

  Pass& operator=(Pass&& _rPass);

private:

  void AddResourceInternal(Resource* _pResource);

private:

  RenderStateInfo m_oInfo;
  uint16_t m_uId;
  uint16_t m_uLayer;

  class Impl;
  std::unique_ptr<Impl> m_pImpl;

};
