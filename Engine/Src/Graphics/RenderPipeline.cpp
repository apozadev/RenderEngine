#include "Graphics/RenderPipeline.h"
#include "Graphics/Window.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"

RenderPipeline::RenderPipeline(const Window* _pWindow, RenderPipelineConfig&& _rConfig)
  : m_pWindow(_pWindow)
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

  GenerateFromConfig();

}

void RenderPipeline::Execute(const std::vector<Job>& _lstJobs, const Camera* _pCamera) const
{
  const Material* pCurrMaterial = nullptr;

  for (const RenderStep& rStep : m_lstRenderSteps)
  {    

    for (const Job& rJob : _lstJobs)
    {

      if (rJob.m_pWindow != _pCamera->GetWindow())
      {
        continue;
      }
      
      const MaterialInstance* pMatInstance = rJob.m_pMaterial;

      const Material* pMaterial = pMatInstance->GetMaterial();

      if (pMaterial != pCurrMaterial)
      {
        pMaterial->Bind();
        _pCamera->Bind();
        rStep.Bind(_pCamera->GetWindow());
        pCurrMaterial = pMaterial;
      }

      rJob.m_pMesh->UpdateTransform(*rJob.m_pMeshTransform);

      pMatInstance->Bind();

      rJob.m_pMesh->Draw();
    }

  }
}

void RenderPipeline::GenerateFromConfig()
{
  static constexpr char* s_sDefaultRtId = "DEFAULT";

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
    std::vector<RenderTarget*> lstInputs(rStepConfig.m_lstInputs.size());

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

    m_lstRenderSteps.push_back(std::move(RenderStep(std::move(lstInputs), pTarget)));
  }
}
