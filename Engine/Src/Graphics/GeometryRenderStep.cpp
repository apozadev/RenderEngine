#include <algorithm>

#include "Graphics/GeometryRenderStep.h"

#include "Graphics/Pass.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Core/Engine.h"

GeometryRenderStep::GeometryRenderStep(const std::string& _sId, std::vector<api::APITexture*>&& _lstInputs, api::APIRenderTarget* _pRenderTarget, bool _bOrderTranslucent, bool _bUseShadowMaps)
  : RenderStep(_sId, std::move(_lstInputs), _pRenderTarget)
  , m_bOrderTranslucent(_bOrderTranslucent)
  ,m_bUseShadowMaps(_bUseShadowMaps)
{
}

void GeometryRenderStep::SetupInternal()
{
  RenderStep::SetupInternal();

  if (m_bUseShadowMaps)
  {
    ResourceBindInfo oBindInfo {};
    oBindInfo.m_eLevel = ResourceFrequency::RENDER_STEP;
    oBindInfo.m_uStageFlags = STAGE_PIXEL;
    oBindInfo.m_sName = "LightBuffer";
    api::SubStateSetupConstantBuffer(ENGINE_API_WINDOW, Renderer::GetInstance()->m_pLightCBuff->m_pAPICbuffer, Renderer::GetInstance()->m_pLightCBuff->GetSize(), oBindInfo);

  }
}

void GeometryRenderStep::SubmitJob(Job&& _rJob)
{
  m_lstJobs.push_back(std::move(_rJob));
}

void GeometryRenderStep::SubmitJobs(const std::vector<Job>& _rJobs)
{
  size_t uCurrSize = m_lstJobs.size();
  m_lstJobs.resize(uCurrSize + _rJobs.size());
  for (size_t i = uCurrSize; i < m_lstJobs.size(); i++)
  {
    m_lstJobs[i] = _rJobs[i-uCurrSize];
  }
}

void GeometryRenderStep::Prepare(const Camera* _pCamera, const Transform* _pViewTransform)
{  
  for (Job& rJob : m_lstJobs)
  {
    rJob.UpdateRenderKey(_pCamera, _pViewTransform, m_bOrderTranslucent);
  }

  std::sort(m_lstJobs.begin(), m_lstJobs.end(), Job::Compare);
}

const Pass* GeometryRenderStep::GetFirstPass() const
{
  if (m_lstJobs.empty()) { return nullptr; }

  return m_lstJobs[0].m_pPass;
}

void GeometryRenderStep::ExecuteInternal(const Camera* _pCamera, const Transform* _pViewTransform)
{

  if (m_lstJobs.empty()) { return; }  

  const Pass* pLastPass = m_lstJobs[0].m_pPass;

  for (const Job& rJob : m_lstJobs)
  {
    if (pLastPass != rJob.m_pPass)
    {      
      rJob.m_pPass->Bind();
      pLastPass = rJob.m_pPass;
    }

    rJob.m_pMesh->UpdateTransform(*rJob.m_pMeshTransform);

    if(rJob.m_pMaterial) rJob.m_pMaterial->Bind(rJob.m_pPass);

    api::DrawMesh(ENGINE_API_WINDOW, rJob.m_pPass->m_pAPIRenderState, rJob.m_pMesh->m_pAPIMesh, rJob.m_pMesh->m_uIndexCount, &rJob.m_pMesh->m_oConstant, sizeof(MeshConstant));
  }

  m_lstJobs.clear();
}
