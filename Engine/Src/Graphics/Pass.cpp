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
#include "Reflection/ReflectionImplMacros.h"

namespace pass_internal
{
  static uint16_t s_uNextId = 0u;
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
  m_oInfo.m_uMeshConstantSize = sizeof(MeshConstant);
  m_oInfo.m_sVSFilename = _sVSFilename;
  m_oInfo.m_sPSFilename = _sPSFilename;
  m_oInfo.m_bBlendEnabled = _bBlendEnabled;
  m_oInfo.m_eBlendOp = _eBlendOp;
  m_oInfo.m_eSrcBlendFactor = _eSrcBlendFactor;
  m_oInfo.m_eDstBlendFactor = _eDstBlendFactor;
  m_oInfo.m_bDepthWrite = _bDepthWrite;
  m_oInfo.m_bDepthRead = _bDepthRead;

  Configure();
  
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
  , int _uStepIdx
  , uint16_t _uLayer)    
{  

  m_sPipelineId = _sPipelineId;

  m_iStepIdx = _uStepIdx;

  RenderPipeline* pPipeline = Renderer::GetInstance()->GetRenderPipeline(_sPipelineId);
  RenderStep* pStep = pPipeline ? pPipeline->GetRenderStep(_uStepIdx) : nullptr;
  const RenderTarget* pTarget = pStep ? pStep->GetRenderTarget() : nullptr;

  Configure(_sVSFilename, _sPSFilename, _bBlendEnabled, _eBlendOp, _eSrcBlendFactor, _eDstBlendFactor, _bDepthWrite, _bDepthRead, pTarget, _uLayer);
}

void Pass::Configure()
{
  RenderPipeline* pPipeline = Renderer::GetInstance()->GetRenderPipeline(m_sPipelineId);
  RenderStep* pStep = pPipeline ? pPipeline->GetRenderStep(m_iStepIdx) : nullptr;
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
    api::SetUsingAPIRenderTarget(nullptr);
    uMsaaSamples = api::GetDefaultMsaaSamples();
  }

  if (m_pAPIRenderState != nullptr)
  {
    api::DestroyAPIRenderState(m_pAPIRenderState);
  }

  m_pAPIRenderState = api::CreateAPIRenderState(m_oInfo, uMsaaSamples);

  // Reflect constant buffers  
  uint32_t uCBuffCount = api::GetConstantBufferCount(m_pAPIRenderState, PipelineStage::PIXEL);

  size_t uCacheIdx = 0u;

  for (uint32_t i = 0u; i < uCBuffCount; i++)
  {
    bool bValid = true;
    uint32_t uNumVariables = api::GetConstantBufferMemberCount(m_pAPIRenderState, PipelineStage::PIXEL, i);

    std::vector<ReflectedConstantBuffer::Variable> lstVariables;
    lstVariables.reserve(uNumVariables);

    size_t uSize = 0u;

    for (uint32_t j = 0u; j < uNumVariables; j++)
    {
      ReflectedConstantBuffer::Variable oVar = {};

      oVar.m_eType = api::GetConstantBufferMemberType(m_pAPIRenderState, PipelineStage::PIXEL, i, j);

      if (oVar.m_eType == ConstantBufferType::NONE)
      {
        bValid = false;
        break;
      }

      uSize += GetConstantBufferTypeSize(oVar.m_eType);

      oVar.m_sName = api::GetConstantBufferMemberName(m_pAPIRenderState, PipelineStage::PIXEL, i, j);

      lstVariables.push_back(std::move(oVar));
    }

    if (bValid)
    {
      std::string sName = api::GetConstantBufferName(m_pAPIRenderState, PipelineStage::PIXEL, i);

      owner_ptr<ReflectedConstantBuffer> pCBuffer = Factory::Create<ReflectedConstantBuffer>();

      size_t uNumFloats = uSize / sizeof(float);

      float* pCache = (uCacheIdx + uNumFloats <= m_lstCBuffCache.size()) ? &(m_lstCBuffCache[uCacheIdx]) : nullptr;      

      pCBuffer->Configure(sName, std::move(lstVariables), pCache);

      uCacheIdx += uNumFloats;

      m_lstCBuffers.push_back(std::move(pCBuffer));      
    }
  }
}

Pass::~Pass()
{  
  api::DestroyAPIRenderState(m_pAPIRenderState);
}

void Pass::Setup() const
{   
  api::BeginRenderStateSetup(m_pAPIRenderState);

  /*for (int i = 0; i < m_lstTextures.size(); i++)
  {
    const owner_ptr<Texture2D>& rTexture = m_lstTextures[i];
    rTexture->SetupRenderSubState(s_aNames[i], PipelineStage::PIXEL, ResourceFrequency::MATERIAL);
  }*/

  for (const owner_ptr<ReflectedConstantBuffer>& pCBuffer : m_lstCBuffers)
  {
    pCBuffer->SetupRenderSubState();
  }

  api::EndRenderStateSetup();
}

void Pass::Bind() const
{

  api::BindAPIRenderState(m_pAPIRenderState);

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
  api::SetUsingAPIRenderState(m_pAPIRenderState);
}

const std::string& Pass::GetRenderPipelineId() const
{
  return m_sPipelineId;
}

int Pass::GetRenderStepIdx() const
{
  return m_iStepIdx;
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

REFLECT_STRUCT_BASE_BEGIN(Pass)
REFLECT_STRUCT_MEMBER(m_oInfo)
REFLECT_STRUCT_MEMBER(m_uLayer)
REFLECT_STRUCT_MEMBER(m_sPipelineId)
REFLECT_STRUCT_MEMBER(m_iStepIdx)
REFLECT_STRUCT_MEMBER(m_lstCBuffCache)
REFLECT_STRUCT_END(Pass)

IMPLEMENT_REFLECTION_POINTER(Pass)

IMPLEMENT_REFLECTION_VECTOR(owner_ptr<Pass>)
