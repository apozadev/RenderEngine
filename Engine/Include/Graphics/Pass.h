#pragma once

#include <memory>

#include "Graphics/Window.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Memory/PtrTypes.h"
#include "Memory/Factory.h"
#include "Core/BaseObject.h"

class Resource;

class Pass : public BaseObject
{

public:    

  ~Pass();

  void Configure(const std::string& _sVSFilename
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

  void AddTexture(owner_ptr<Texture2D>&& _pTexture)
  {
    m_lstTextures.push_back(std::move(_pTexture));    
  }

  template<typename T>
  ConstantBuffer<T> AddConstantBuffer()
  {        
    m_lstCBuffers.push_back(Factory::Create<ConstantBuffer<T>>().cast_release<ConstantBufferBase>());
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

private:

  RenderStateInfo m_oInfo;

  uint16_t m_uId;

  uint16_t m_uLayer;

  api::APIRenderState* m_pAPIRenderState;

  std::vector<owner_ptr<Texture2D>> m_lstTextures;

  std::vector<owner_ptr<ConstantBufferBase>> m_lstCBuffers;

  std::string m_sPipelineId;

  int m_iStepIdx;

};
