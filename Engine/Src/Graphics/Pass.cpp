#include "Graphics/Pass.h"

#include <string>

#include "Graphics/Resource.h"
#include "Graphics/Window.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderStep.h"
#include "Graphics/RenderTarget.h"
#include "Core/Exception.h"
#include "Core/Engine.h"
#include "Reflection/ReflectionImplMacros.h"

namespace pass_internal
{
  static uint16_t s_uNextId = 0u;
}

void Pass::Configure(const RenderStateInfo& _rRenderStateInfo, const RenderTarget* _pRenderTarget, uint32_t _uLayer)
{
  m_uLayer = _uLayer;

  m_oInfo = _rRenderStateInfo;

  Configure(_pRenderTarget);
}

void Pass::Configure(const std::string& _sVSFilename
  , const std::string& _sPSFilename
  , bool _bBlendEnabled
  , BlendOp _eBlendOp
  , BlendFactor _eSrcBlendFactor
  , BlendFactor _eDstBlendFactor
  , bool _bDepthWrite
  , bool _bDepthRead
  , const RenderTarget* _pRenderTarget
  , uint16_t _uLayer)
{
  m_uId = pass_internal::s_uNextId++;

  m_uLayer = _uLayer;  

  m_oInfo = {};
  m_oInfo.m_sVSFilename = _sVSFilename;
  m_oInfo.m_sPSFilename = _sPSFilename;
  m_oInfo.m_bBlendEnabled = _bBlendEnabled;
  m_oInfo.m_eBlendOp = _eBlendOp;
  m_oInfo.m_eSrcBlendFactor = _eSrcBlendFactor;
  m_oInfo.m_eDstBlendFactor = _eDstBlendFactor;
  m_oInfo.m_bDepthWrite = _bDepthWrite;
  m_oInfo.m_bDepthRead = _bDepthRead;

  Configure(_pRenderTarget);
  
}

void Pass::Configure(const std::string& _sVSFilename
  , const std::string& _sPSFilename
  , bool _bBlendEnabled
  , BlendOp _eBlendOp
  , BlendFactor _eSrcBlendFactor
  , BlendFactor _eDstBlendFactor
  , bool _bDepthWrite
  , bool _bDepthRead
  , const std::string& _sPipelineId
  , const std::string& _sStepId
  , uint16_t _uLayer)    
{  

  m_sPipelineId = _sPipelineId; 
  m_sStepId = _sStepId;

  RenderPipeline* pPipeline = Renderer::GetInstance()->GetRenderPipeline(_sPipelineId);
  RenderStep* pStep = pPipeline ? pPipeline->GetRenderStep(m_sStepId) : nullptr;
  const RenderTarget* pTarget = pStep ? pStep->GetRenderTarget() : nullptr;

  Configure(_sVSFilename, _sPSFilename, _bBlendEnabled, _eBlendOp, _eSrcBlendFactor, _eDstBlendFactor, _bDepthWrite, _bDepthRead, pTarget, _uLayer);
}

void Pass::Configure()
{
  RenderPipeline* pPipeline = Renderer::GetInstance()->GetRenderPipeline(m_sPipelineId);
  RenderStep* pStep = pPipeline ? pPipeline->GetRenderStep(m_sStepId) : nullptr;
  const RenderTarget* pTarget = pStep ? pStep->GetRenderTarget() : nullptr;

  Configure(pTarget);
}

void Pass::Configure(const RenderTarget* _pRenderTarget)
{
  uint32_t uMsaaSamples = 1u;

  // If pTarget is null, we are drawing to the backbuffer
  if (_pRenderTarget)
  {
    _pRenderTarget->SetUsing();
    uMsaaSamples = _pRenderTarget->GetMsaaSamples();
  }
  else
  {
    uMsaaSamples = api::GetWindowMSAASamples(ENGINE_API_WINDOW);
  }

  if (m_pAPIRenderState != nullptr)
  {
    api::DestroyAPIRenderState(ENGINE_API_WINDOW, m_pAPIRenderState);
  }

  m_oInfo.m_uMeshConstantSize = sizeof(MeshConstant);

  m_pAPIRenderState = api::CreateAPIRenderState(ENGINE_API_WINDOW, m_oInfo, _pRenderTarget ? _pRenderTarget->m_pAPIRenderTarget : nullptr, uMsaaSamples);

  // Reflect constant buffers  
  ReflectCBuffers(STAGE_VERTEX);
  ReflectCBuffers(STAGE_PIXEL);
  ReflectCBuffers(STAGE_GEOM);
}

void Pass::ReflectCBuffers(PipelineStage _eStage)
{
  uint32_t uCBuffCount = api::GetConstantBufferCount(m_pAPIRenderState, _eStage);

  size_t uCacheIdx = 0u;

  for (uint32_t i = 0u; i < uCBuffCount; i++)
  {
    bool bValid = true;
    uint32_t uNumVariables = api::GetConstantBufferMemberCount(m_pAPIRenderState, _eStage, i);

    std::vector<ReflectedConstantBuffer::Variable> lstVariables;
    lstVariables.reserve(uNumVariables);

    size_t uSize = 0u;

    for (uint32_t j = 0u; j < uNumVariables; j++)
    {
      ReflectedConstantBuffer::Variable oVar = {};

      oVar.m_eType = api::GetConstantBufferMemberType(m_pAPIRenderState, _eStage, i, j);

      oVar.m_uArraySize = api::GetConstantBufferMemberArraySize(m_pAPIRenderState, _eStage, i, j);

      if (oVar.m_eType == ConstantBufferType::NONE)
      {
        bValid = false;
        break;
      }

      uSize += GetConstantBufferTypeSize(oVar.m_eType) * oVar.m_uArraySize;

      oVar.m_sName = api::GetConstantBufferMemberName(m_pAPIRenderState, _eStage, i, j);

      lstVariables.push_back(std::move(oVar));
    }

    if (bValid)
    {
      std::string sName = api::GetConstantBufferName(m_pAPIRenderState, _eStage, i);

      owner_ptr<ReflectedConstantBuffer> pCBuffer = Factory::Create<ReflectedConstantBuffer>();

      size_t uNumFloats = uSize / sizeof(float);

      float* pCache = (uCacheIdx + uNumFloats <= m_lstCBuffCache.size()) ? &(m_lstCBuffCache[uCacheIdx]) : nullptr;

      pCBuffer->Configure(sName, _eStage, std::move(lstVariables), pCache);

      uCacheIdx += uNumFloats;

      m_lstCBuffers.push_back(std::move(pCBuffer));
    }
  }
}

Pass::~Pass()
{  
  api::DestroyAPIRenderState(ENGINE_API_WINDOW, m_pAPIRenderState);
}

void Pass::Setup() const
{   
  api::BeginRenderStateSetup(m_pAPIRenderState);

  /*for (int i = 0; i < m_lstTextures.size(); i++)
  {
    const owner_ptr<Texture2D>& rTexture = m_lstTextures[i];
    rTexture->SetupRenderSubState(s_aNames[i], STAGE_PIXEL, ResourceFrequency::MATERIAL);
  }*/

  for (const owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    ResourceBindInfo oBindInfo{};
    oBindInfo.m_eLevel = ResourceFrequency::MATERIAL;
    oBindInfo.m_uStageFlags = pCBuffer->GetPipelineStage();
    oBindInfo.m_sName = pCBuffer->GetName();
    api::RenderStateSetupConstantBuffer(ENGINE_API_WINDOW, pCBuffer->m_pAPICbuffer, pCBuffer->GetSize(), oBindInfo, m_pAPIRenderState);
  }

  api::EndRenderStateSetup(ENGINE_API_WINDOW);
}

void Pass::Bind() const
{

  api::BindAPIRenderState(ENGINE_API_WINDOW, m_pAPIRenderState);

  /*for (const owner_ptr<Texture2D>& pTexture : m_lstTextures)
  {
    pTexture->Bind();
  }*/

  for (const owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    pCBuffer->Update();
    pCBuffer->Bind();
  }
}

void Pass::SetUsing() const
{
}

bool Pass::GetFloat(const char* _sName, float* pOutValue_) const 
{
  for (const owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    if (pCBuffer->GetFloat(_sName, pOutValue_))
    {
      return true;
    }
  }

  return false;
}

bool Pass::GetVec4(const char* _sName, float* pOutValue_) const
{
  for (const owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    if (pCBuffer->GetVec4(_sName, pOutValue_))
    {
      return true;
    }
  }

  return false;
}

bool Pass::GetMat4(const char* _sName, float* pOutValue_) const
{
  for (const owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    if (pCBuffer->GetMat4(_sName, pOutValue_))
    {
      return true;
    }
  }

  return false;
}

bool Pass::SetFloat(const char* _sName, float _fValue)
{
  for (owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    if (pCBuffer->SetFloat(_sName, _fValue))
    {
      return true;
    }
  }

  return false;
}

bool Pass::SetVec4(const char* _sName, float* _pValue)
{
  for (owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    if (pCBuffer->SetVec4(_sName, _pValue))
    {
      return true;
    }
  }

  return false;
}

bool Pass::SetMat4(const char* _sName, float* _pValue)
{
  for (owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    if (pCBuffer->SetMat4(_sName, _pValue))
    {
      return true;
    }
  }

  return false;
}

void Pass::UpdateCache()
{
  m_lstCBuffCache.clear();

  for (auto& rCBuff : m_lstCBuffers)
  {
    for (auto& rVar : rCBuff->GetVariables())
    {
      switch (rVar.m_eType)
      {
      case ConstantBufferType::SCALAR:
      {
        float fValue = 0.f;
        GetFloat(rVar.m_sName.c_str(), &fValue);
        m_lstCBuffCache.push_back(fValue);
      }
        break;
      case ConstantBufferType::VEC4:
      {
        float aValues[4] = { 0.f, 0.f, 0.f, 0.f };
        GetVec4(rVar.m_sName.c_str(), &aValues[0]);
        m_lstCBuffCache.push_back(aValues[0]);
        m_lstCBuffCache.push_back(aValues[1]);
        m_lstCBuffCache.push_back(aValues[2]);
        m_lstCBuffCache.push_back(aValues[3]);
      }
        break;
      }
    }
  }
}

REFLECT_STRUCT_BASE_BEGIN(Pass)
REFLECT_STRUCT_MEMBER(m_oInfo)
REFLECT_STRUCT_MEMBER(m_uLayer)
REFLECT_STRUCT_MEMBER(m_sPipelineId)
REFLECT_STRUCT_MEMBER(m_sStepId)
REFLECT_STRUCT_MEMBER(m_lstCBuffCache)
REFLECT_STRUCT_END(Pass)

IMPLEMENT_REFLECTION_POINTER(Pass)

IMPLEMENT_REFLECTION_VECTOR(owner_ptr<Pass>)
