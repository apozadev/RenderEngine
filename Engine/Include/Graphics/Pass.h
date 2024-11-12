#pragma once

#include <memory>

#include "Graphics/Window.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Core/PooledObject.h"

class Resource;

#define PASS_POOL_SIZE 256

class Pass : public PooledObject<Pass, PASS_POOL_SIZE>
{

public:  

  using PooledObject<Pass, PASS_POOL_SIZE>::PooledObject;

  ~Pass();

  void Initialize(const std::string& _sVSFilename
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

private:

  void AddResourceInternal(Resource* _pResource);

private:

  RenderStateInfo m_oInfo;

  uint16_t m_uId;

  uint16_t m_uLayer;

  api::APIRenderState* m_pAPIRenderState;

  std::vector<Resource*> m_lstResources;

  std::string m_sPipelineId;

  int m_iStepIdx;

};
