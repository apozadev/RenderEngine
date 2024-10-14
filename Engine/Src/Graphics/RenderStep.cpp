#include "Graphics/RenderStep.h"

#include <vector>
#include <algorithm>

#include "Math/Transform.h"

#include "Graphics/RenderTarget.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Pass.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"
#include "Graphics/Job.h"
#include "Graphics/API/GraphicsAPI.h"

class RenderStep::Impl
{
public:

  Impl(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget)
    : m_lstInputs(std::move(_lstInputs))
    , m_pRenderTarget(_pRenderTarget)
  {    

    m_pAPIRenderSubState = api::CreateAPIRenderSubState(ResourceFrequency::RENDER_STEP);

    api::BeginSubStateSetup(m_pAPIRenderSubState);

    for (RenderTarget* _pInput : m_lstInputs)
    {
      _pInput->GetColorTextures()[0]->SetupRenderSubState(ResourceFrequency::RENDER_STEP);
    }

    api::EndSubStateSetup(ResourceFrequency::RENDER_STEP);

  }

  ~Impl()
  {
    api::DestroyRenderSubState(m_pAPIRenderSubState);
  }

  std::vector<Job> m_lstJobs;

  std::vector<RenderTarget*> m_lstInputs;

  const RenderTarget* m_pRenderTarget;

  api::APIRenderSubState* m_pAPIRenderSubState;

};

RenderStep::RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget, bool _bOrderTranslucent)
  : m_bOrderTranslucent(_bOrderTranslucent)
{
  m_pImpl = std::make_unique<Impl>(std::move(_lstInputs), _pRenderTarget);
}

RenderStep::RenderStep(RenderStep&& _rOther)
  : m_pImpl(std::move(_rOther.m_pImpl))
  , m_bOrderTranslucent(std::move(_rOther.m_bOrderTranslucent))
{
}

RenderStep::~RenderStep()
{
}

void RenderStep::SubmitJob(Job&& _rJob)
{
  m_pImpl->m_lstJobs.push_back(std::move(_rJob));
}

void RenderStep::Execute(const Camera* _pCamera, const Transform* _pViewTransform) const
{

  // Update key for current camera
  for (Job& rJob : m_pImpl->m_lstJobs)
  {
    rJob.UpdateRenderKey(_pCamera, _pViewTransform, m_bOrderTranslucent);
  }

  // Sort jobs
  std::sort(m_pImpl->m_lstJobs.begin(), m_pImpl->m_lstJobs.end(), Job::Compare);

  bool bStepBound = false;

  const Pass* pLastPass = nullptr;  

  for (const Job& rJob : m_pImpl->m_lstJobs)
  {

    if (rJob.m_pWindow != _pCamera->GetWindow())
    {
      continue;
    }

    const MaterialInstance* pMatInstance = rJob.m_pMaterial;

    const Pass* pPass = rJob.m_pPass;

    if (pPass != pLastPass)
    {
      pPass->Bind();

      if (!bStepBound)
      {
        _pCamera->Bind();
        Bind(_pCamera->GetWindow());
        bStepBound = true;
      }

      pLastPass = pPass;
    }

    rJob.m_pMesh->UpdateTransform(*rJob.m_pMeshTransform);

    pMatInstance->Bind();

    rJob.m_pMesh->Draw();
  }

  Unbind();
}

void RenderStep::Clear()
{
  m_pImpl->m_lstJobs.clear();
}

const RenderTarget* RenderStep::GetRenderTarget() const
{
  return m_pImpl->m_pRenderTarget;
}

void RenderStep::Bind(const Window* _pWindow) const
{

  if (m_pImpl->m_pRenderTarget)
  {
    m_pImpl->m_pRenderTarget->Bind();
  }
  else
  {
    _pWindow->BindDefaultRenderTarget();
  }

  for (RenderTarget* _pInput : m_pImpl->m_lstInputs)
  {
    _pInput->GetColorTextures()[0]->Bind();
  }

  api::BindAPIRenderSubState(m_pImpl->m_pAPIRenderSubState, ResourceFrequency::RENDER_STEP);  
}

void RenderStep::Unbind() const
{
  if (m_pImpl->m_pRenderTarget)
  {
    m_pImpl->m_pRenderTarget->Unbind();
  }
}

