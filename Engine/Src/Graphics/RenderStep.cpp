#include "Graphics/RenderStep.h"

#include <vector>

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


RenderStep::RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget)
  : m_lstInputs(std::move(_lstInputs))
  , m_pRenderTarget(_pRenderTarget)
{
  m_pAPIRenderSubState = api::CreateAPIRenderSubState(ResourceFrequency::RENDER_STEP);
}

RenderStep::~RenderStep()
{
  api::DestroyRenderSubState(m_pAPIRenderSubState);
}

void RenderStep::Setup() 
{   
  api::BeginSubStateSetup(m_pAPIRenderSubState);

  SetupInternal();

  api::EndSubStateSetup(ResourceFrequency::RENDER_STEP);
}

void RenderStep::SetupInternal()
{
  for (RenderTarget* _pInput : m_lstInputs)
  {
    _pInput->GetColorTextures()[0]->SetupRenderSubState(ResourceFrequency::RENDER_STEP);
  }
}

void RenderStep::Execute(const Camera* _pCamera, const Transform* _pViewTransform)
{

  Prepare(_pCamera, _pViewTransform);

  const Pass* pPass = GetFirstPass();

  if (pPass != nullptr)
  {
    pPass->Bind();

    _pCamera->Bind();

    Bind();

    ExecuteInternal(_pCamera, _pViewTransform);

    Unbind();
  }
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

