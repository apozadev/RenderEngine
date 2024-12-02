#include "Graphics/RenderPipeline.h"
#include "Core/Engine.h"
#include "Graphics/Window.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"

RenderPipeline::RenderPipeline(std::string _sId, RenderPipelineConfig&& _rConfig)
  : m_sId(_sId)
  , m_oConfig(std::move(_rConfig))
{
  GenerateFromConfig();
}

void RenderPipeline::OnWindowResize()
{

  m_lstRenderSteps.clear();
  m_lstRenderTargets.clear();

  GenerateFromConfig();

}

void RenderPipeline::Execute(const Camera* _pCamera, const Transform* _pViewTransform)
{        

  for (owner_ptr<RenderTarget>& pRenderTarget : m_lstRenderTargets)
  {
    pRenderTarget->Clear();
  }

  for (RenderStep& rStep : m_lstRenderSteps)
  {                
    rStep.Execute(_pCamera, _pViewTransform);
  }
}

void RenderPipeline::Clear()
{
  for (RenderStep& rStep : m_lstRenderSteps)
  {
    rStep.Clear();
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
    std::vector<RenderTarget*> lstInputs;

    for (const RenderStepInputConfig& rInputConfig : rStepConfig.m_lstInputs)
    {
      for (int i = 0; i < m_oConfig.m_lstRenderTargets.size(); i++)
      {
        if (rInputConfig.m_sTargetId == m_oConfig.m_lstRenderTargets[i].m_sId)
        {
          lstInputs.push_back(m_lstRenderTargets[i].get());
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

    m_lstRenderSteps.push_back(std::move(RenderStep(std::move(lstInputs), pTarget, rStepConfig.m_bOrderTranslucent)));
  }
}
