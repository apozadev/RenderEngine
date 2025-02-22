#pragma once

#include <memory>
#include <string>

#include "Graphics/Window.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ReflectedConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Memory/PtrTypes.h"
#include "Memory/Factory.h"
#include "Core/BaseObject.h"
#include "Reflection/ReflectionMacros.h"

class RenderTarget;

class Pass : public BaseObject
{

public:    

  REFLECT_BASE()

  Pass() = default;  

  ~Pass();

  void Configure(const std::string& _sVSFilename
    , const std::string& _sPSFilename
    , bool _bBlendEnabled
    , BlendOp _eBlendOp
    , BlendFactor _eSrcBlendFactor
    , BlendFactor _eDstBlendFactor
    , bool _bDepthWrite
    , bool _bDepthRead
    , const RenderTarget* _pRenderTarget
    , uint16_t _uLayer);

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

  void Configure() override;

  /*void AddTexture(owner_ptr<Texture2D>&& _pTexture)
  {
    m_lstTextures.push_back(std::move(_pTexture));    
  }*/

  /*template<typename T>
  ConstantBuffer<T> AddConstantBuffer()
  {        
    owner_ptr<ConstantBuffer<T>> pCBuff = Factory::Create<ConstantBuffer<T>>();
    pCBuff->Configure();
    m_lstCBuffers.push_back(pCBuff.cast_release<ConstantBufferBase>());
  }*/

  void Setup() const;

  void Bind() const;

  void SetUsing() const;

  const RenderStateInfo& GetRenderStateInfo() const { return m_oInfo; }

  uint16_t GetId() const { return m_uId; }

  uint16_t GetLayer() const { return m_uLayer; }

  const std::string& GetRenderPipelineId() const;

  int GetRenderStepIdx() const;

  bool GetFloat(const char* _sName, float* pOutValue_) const;
  bool GetVec4(const char* _sName, float* pOutValue_) const;

  bool SetFloat(const char* _sName, float _fValue);
  bool SetVec4(const char* _sName, float* _pValue);

  Pass& operator=(const Pass&) = delete;

private:  

  void Configure(const RenderTarget* _pRenderTarget);

private:

  RenderStateInfo m_oInfo;

  uint16_t m_uId;

  uint16_t m_uLayer;

  api::APIRenderState* m_pAPIRenderState;
  api::APIRenderState* m_pShadowAPIRenderState; 

  //std::vector<owner_ptr<Texture2D>> m_lstTextures;

  std::vector<owner_ptr<ReflectedConstantBuffer>> m_lstCBuffers;

  std::vector<float> m_lstCBuffCache;

  std::string m_sPipelineId;

  int m_iStepIdx;  

};

DECLARE_REFLECTION_POINTER(Pass)

DECLARE_REFLECTION_VECTOR(owner_ptr<Pass>)
