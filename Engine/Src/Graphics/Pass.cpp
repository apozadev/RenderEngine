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
  , const std::string& _sPipelineId
  , int _uStepIdx
  , uint16_t _uLayer)    
{  
  m_uId = pass_internal::s_uNextId++;

  m_uLayer = _uLayer;

  m_sPipelineId = _sPipelineId;

  m_iStepIdx = static_cast<int>(_uStepIdx);

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

  RenderPipeline* pPipeline = Renderer::GetInstance()->GetRenderPipeline(_sPipelineId);
  RenderStep* pStep = pPipeline ? pPipeline->GetRenderStep(_uStepIdx) : nullptr;
  const RenderTarget* pTarget = pStep ? pStep->GetRenderTarget() : nullptr;

  uint32_t uMsaaSamples = 1u;

  // If pTarget is null, we are drawing to the backbuffer
  if (pTarget)
  {
    pTarget->SetUsing();
    uMsaaSamples = pTarget->GetMsaaSamples();
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
}

Pass::~Pass()
{  
  api::DestroyAPIRenderState(m_pAPIRenderState);
}

void Pass::Setup() const
{
  api::BeginRenderStateSetup(m_pAPIRenderState);

  for (const owner_ptr<Texture2D>& pTexture : m_lstTextures)
  {
    pTexture->SetupRenderSubState(ResourceFrequency::MATERIAL);
  }

  for (const owner_ptr<ConstantBufferBase>& pCBuffer : m_lstCBuffers)
  {
    pCBuffer->SetupRenderSubState(ResourceFrequency::MATERIAL);
  }

  api::EndRenderStateSetup();
}

void Pass::Bind() const
{

  api::BindAPIRenderState(m_pAPIRenderState);

  for (const owner_ptr<Texture2D>& pTexture : m_lstTextures)
  {
    pTexture->Bind();
  }

  for (const owner_ptr<ConstantBufferBase>& pCBuffer : m_lstCBuffers)
  {
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
