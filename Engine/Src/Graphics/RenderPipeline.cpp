#include "Graphics/RenderPipeline.h"
#include "Graphics/Window.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"

RenderPipeline::RenderPipeline(const Window* _pWindow, std::string _sId, RenderPipelineConfig&& _rConfig)
  : m_pWindow(_pWindow)
  , m_sId(_sId)
  , m_oConfig(std::move(_rConfig))
{
  GenerateFromConfig();
}

RenderPipeline::~RenderPipeline()
{
  for (RenderTarget* pRenderTarget : m_lstRenderTargets)
  {
    delete pRenderTarget;
  }
}

void RenderPipeline::OnWindowResize()
{
  for (RenderTarget* pRenderTarget : m_lstRenderTargets)
  {
    delete pRenderTarget;
  }

  m_lstRenderSteps.clear();
  m_lstRenderTargets.clear();

  GenerateFromConfig();

}

void RenderPipeline::Execute(const Camera* _pCamera, const Transform* _pViewTransform) const
{        
  for (const RenderStep& rStep : m_lstRenderSteps)
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

  m_pWindow->SetUsing();

  for (const RenderTargetConfig& rRtConfig : m_oConfig.m_lstRenderTargets)
  {
    unsigned int uWidth = rRtConfig.m_iWidth < 0 ? m_pWindow->GetWidth() : rRtConfig.m_iWidth;
    unsigned int uHeight = rRtConfig.m_iHeight < 0 ? m_pWindow->GetHeight() : rRtConfig.m_iHeight;
    m_lstRenderTargets.push_back(new RenderTarget(
      rRtConfig.m_uNumColorTextures
      , uWidth
      , uHeight
      , rRtConfig.m_eFormat
      , rRtConfig.m_bHasDepthStencil
      , rRtConfig.m_uMipLevels
      , rRtConfig.m_uMsaaSamples));
  }

  for (const RenderStepConfig& rStepConfig : m_oConfig.m_lstSteps)
  {
    std::vector<RenderTarget*> lstInputs;

    for (const RenderStepInputConfig& rInputConfig : rStepConfig.m_lstInputs)
    {
      for (int i = 0; i < m_oConfig.m_lstRenderTargets.size(); i++)
      {
        if (rInputConfig.m_sTargetId == m_oConfig.m_lstRenderTargets[i].m_sId)
        {
          lstInputs.push_back(m_lstRenderTargets[i]);
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
          pTarget = m_lstRenderTargets[i];
        }
      }
    }

    m_lstRenderSteps.push_back(std::move(RenderStep(std::move(lstInputs), pTarget, rStepConfig.m_bOrderTranslucent)));
  }
}
