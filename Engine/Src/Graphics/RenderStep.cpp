#include "Graphics/RenderStep.h"

#include <vector>
#include <algorithm>

#include "Math/Transform.h"

#include "Core/Engine.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Pass.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Mesh.h"
#include "Graphics/API/GraphicsAPI.h"


RenderStep::RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget, bool _bOrderTranslucent)
  : m_bOrderTranslucent(_bOrderTranslucent)
  , m_lstInputs(std::move(_lstInputs))
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

RenderStep::~RenderStep()
{
  api::DestroyRenderSubState(m_pAPIRenderSubState);

}

void RenderStep::SubmitJob(Job&& _rJob)
{
  m_lstJobs.push_back(std::move(_rJob));
}

void RenderStep::Execute(const Camera* _pCamera, const Transform* _pViewTransform)
{

  // Update key for current camera
  for (Job& rJob : m_lstJobs)
  {
    rJob.UpdateRenderKey(_pCamera, _pViewTransform, m_bOrderTranslucent);
  }

  // Sort jobs
  std::sort(m_lstJobs.begin(), m_lstJobs.end(), Job::Compare);

  bool bStepBound = false;

  const Pass* pLastPass = nullptr;  

  for (const Job& rJob : m_lstJobs)
  {

    const MaterialInstance* pMatInstance = rJob.m_pMaterial;

    const Pass* pPass = rJob.m_pPass;

    if (pPass != pLastPass)
    {
      pPass->Bind();

      if (!bStepBound)
      {
        _pCamera->Bind();
        Bind();
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
  m_lstJobs.clear();
}

const RenderTarget* RenderStep::GetRenderTarget() const
{
  return m_pRenderTarget;
}

void RenderStep::Bind() const
{

  if (m_pRenderTarget)
  {
    m_pRenderTarget->Bind();
  }
  else
  {
    Engine::GetInstance()->GetWindow()->BindDefaultRenderTarget();
  }

  for (RenderTarget* _pInput : m_lstInputs)
  {
    _pInput->GetColorTextures()[0]->Bind();
  }

  api::BindAPIRenderSubState(m_pAPIRenderSubState, ResourceFrequency::RENDER_STEP);  
}

void RenderStep::Unbind() const
{
  if (m_pRenderTarget)
  {
    m_pRenderTarget->Unbind();
  }
  else
  {
    Engine::GetInstance()->GetWindow()->UnbindDefaultRenderTarget();
  }
}

