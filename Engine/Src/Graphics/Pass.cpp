#include "Graphics/Pass.h"

#include <string>

#include "Graphics/Resource.h"
#include "Graphics/API/GraphicsAPI.h"
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

class Pass::Impl
{
public:
  api::APIRenderState* m_pAPIRenderState;
  std::vector<Resource*> m_lstResources;  
  std::string m_sPipelineId;
  int m_iStepIdx;

  Impl(Window* _pWindow, const RenderStateInfo& _rInfo, const std::string& _sPipelineId, int _uStepIdx)
    : m_sPipelineId(_sPipelineId)
    , m_iStepIdx(_uStepIdx)
  {
    _pWindow->SetUsing();

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

    m_pAPIRenderState = api::CreateAPIRenderState(_rInfo, uMsaaSamples);
  }

  ~Impl()
  {
    for (Resource* pResource : m_lstResources)
    {
      delete pResource;
    }

    api::DestroyAPIRenderState(m_pAPIRenderState);
  }
};

Pass::Pass(Window* _pWindow
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

  m_pImpl = std::make_unique<Impl>(_pWindow, m_oInfo, _sPipelineId, _uStepIdx);
}

Pass::Pass(Pass&& rPass)
  : m_pImpl(std::move(rPass.m_pImpl))
  , m_oInfo(std::move(rPass.m_oInfo))
  , m_uId(std::move(rPass.m_uId))
  , m_uLayer(std::move(rPass.m_uLayer))
{
}

Pass::~Pass()
{
}

void Pass::Setup() const
{
  api::BeginRenderStateSetup(m_pImpl->m_pAPIRenderState);

  for (Resource* pResource : m_pImpl->m_lstResources)
  {
    pResource->SetupRenderSubState(ResourceFrequency::MATERIAL);
  }

  api::EndRenderStateSetup();
}

void Pass::Bind() const
{

  api::BindAPIRenderState(m_pImpl->m_pAPIRenderState);

  for (const Resource* pResource : m_pImpl->m_lstResources)
  {
    pResource->Bind();
  }
}

void Pass::SetUsing() const
{
  api::SetUsingAPIRenderState(m_pImpl->m_pAPIRenderState);
}

void Pass::AddResourceInternal(Resource* _pResource)
{
  m_pImpl->m_lstResources.push_back(_pResource);
}

const std::string& Pass::GetRenderPipelineId() const
{
  return m_pImpl->m_sPipelineId;
}

int Pass::GetRenderStepIdx() const
{
  return m_pImpl->m_iStepIdx;
}

Pass& Pass::operator=(Pass&& _rPass)
{
  m_pImpl = std::move(_rPass.m_pImpl);
  return *this;
}
