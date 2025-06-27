#include "Graphics/RenderPipeline.h"
#include "Core/Engine.h"
#include "Graphics/Window.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"
#include "Graphics/GeometryRenderStep.h"

RenderPipeline::RenderPipeline(std::string _sId, RenderPipelineConfig&& _rConfig)
  : m_sId(_sId)
  , m_oConfig(std::move(_rConfig))
{
  GenerateFromConfig();
}

RenderStep* RenderPipeline::GetRenderStep(const std::string& _sId)
{
  for (owner_ptr<RenderStep>& pStep : m_lstRenderSteps)
  {
    if (pStep->GetId() == _sId)
    {
      return pStep.get();
    }
  }

  return nullptr;
}

void RenderPipeline::OnWindowResize()
{

  m_lstRenderSteps.clear();
  m_lstRenderTargets.clear();

  GenerateFromConfig();

  Setup();
}

void RenderPipeline::Setup()
{
  for (owner_ptr<RenderStep>& rStep : m_lstRenderSteps)
  {
    rStep->Setup();
  }
}

void RenderPipeline::Execute(const Camera* _pCamera, const Transform* _pViewTransform)
{        

  for (owner_ptr<RenderTarget>& pRenderTarget : m_lstRenderTargets)
  {
    pRenderTarget->Clear();
  }

  api::APIRenderTarget* pLastRT = nullptr;
  bool bLastRtIsDefault = false;

  bool bDoBindRT = true;

  for (unsigned int i = 0u; i < m_lstRenderSteps.size(); i++)
  {    

    owner_ptr<RenderStep>& pStep = m_lstRenderSteps[i];    

    // Unbind last RT if necessary
    if (pLastRT != pStep->m_pRenderTarget && pLastRT != nullptr)
    {
      api::UnbindAPIRenderTarget(ENGINE_API_WINDOW, pLastRT);
      bDoBindRT = true;
    }
    else if (bLastRtIsDefault && pStep->m_pRenderTarget != nullptr)
    {
      api::UnbindDefaultRenderTarget(ENGINE_API_WINDOW);
      bDoBindRT = true;
    }     
    
    pStep->Execute(_pCamera, _pViewTransform, bDoBindRT);

    pLastRT = pStep->m_pRenderTarget;
    bLastRtIsDefault = pLastRT == nullptr;    
    bDoBindRT = false;
  }

  // Unbind last RT
  if (pLastRT != nullptr)
  {
    api::UnbindAPIRenderTarget(ENGINE_API_WINDOW, pLastRT);
  }
  else if (bLastRtIsDefault)
  {
    Engine::GetInstance()->GetWindow()->UnbindDefaultRenderTarget();
  }
}

void RenderPipeline::GenerateFromConfig()
{
  static constexpr char* s_sDefaultRtId = "DEFAULT";  

  for (const RenderTargetConfig& rRtConfig : m_oConfig.m_lstRenderTargets)
  {
    unsigned int uWidth = rRtConfig.m_iWidth < 0 ? Engine::GetInstance()->GetWindow()->GetWidth() : rRtConfig.m_iWidth;
    unsigned int uHeight = rRtConfig.m_iHeight < 0 ? Engine::GetInstance()->GetWindow()->GetHeight() : rRtConfig.m_iHeight;

    owner_ptr<RenderTarget> pRenderTarget = Factory::Create<RenderTarget>();

    pRenderTarget->Configure(
      rRtConfig.m_uNumColorTextures
      , uWidth
      , uHeight
      , rRtConfig.m_eFormat
      , rRtConfig.m_bHasDepthStencil
      , rRtConfig.m_uMipLevels
      , rRtConfig.m_uMsaaSamples);

    m_lstRenderTargets.push_back(std::move(pRenderTarget));
  }

  m_lstRenderSteps.reserve(m_oConfig.m_lstSteps.size());

  for (const RenderStepConfig& rStepConfig : m_oConfig.m_lstSteps)
  {
    std::vector<Texture2D*> lstInputs;

    for (const RenderStepInputConfig& rInputConfig : rStepConfig.m_lstInputs)
    {
      for (int i = 0; i < m_oConfig.m_lstRenderTargets.size(); i++)
      {
        if (rInputConfig.m_sTargetId == m_oConfig.m_lstRenderTargets[i].m_sId)
        {
          lstInputs.push_back(m_lstRenderTargets[i]->GetColorTextures()[0].get());
          break;
        }
      }
    }

    RenderTarget* pTarget = nullptr;
    // If target Id is DEFAULT, target will stay null
    if (rStepConfig.m_sRenderTargetId != s_sDefaultRtId)
    {
      for (int i = 0; i < m_oConfig.m_lstRenderTargets.size(); i++)
      {
        if (rStepConfig.m_sRenderTargetId == m_oConfig.m_lstRenderTargets[i].m_sId)
        {
          pTarget = m_lstRenderTargets[i].get();
        }
      }
    }     

    owner_ptr<GeometryRenderStep> pRenderStep = Factory::Create<GeometryRenderStep>(rStepConfig.m_sId, std::move(lstInputs), pTarget ? pTarget->m_pAPIRenderTarget : nullptr, rStepConfig.m_bOrderTranslucent);
    m_lstRenderSteps.push_back(pRenderStep.cast_release<RenderStep>()); 
  }
}
