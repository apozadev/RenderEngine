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


RenderStep::RenderStep(const std::string& _sId, std::vector<Texture2D*>&& _lstInputs, const RenderTarget* _pRenderTarget)
  : m_sId(_sId)
  , m_lstInputs(std::move(_lstInputs))
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

  static const char s_aNames[4][7] = { "Input0", "Input1", "Input2", "Input3" };

  if (m_lstInputs.size() > 4)
  {
    THROW_GENERIC_EXCEPTION("RenderStep has more than 4 inputs")
  }

  for (int i = 0; i < m_lstInputs.size(); i++)
  {    
    m_lstInputs[i]->SetupRenderSubState(s_aNames[i], PipelineStage::PIXEL, ResourceFrequency::RENDER_STEP);
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
    if (!m_pRenderTarget->IsBound())
    {
      m_pRenderTarget->Bind();
    }
  }
  else
  {
    if (!Engine::GetInstance()->GetWindow()->IsDefaultRenderTargetBound())
    {
      Engine::GetInstance()->GetWindow()->BindDefaultRenderTarget();
    }
  }

  for (Texture2D* _pInput : m_lstInputs)
  {
    _pInput->Bind();
  }  

  api::BindAPIRenderSubState(m_pAPIRenderSubState, ResourceFrequency::RENDER_STEP);
}

void RenderStep::Unbind() const
{

  for (Texture2D* _pInput : m_lstInputs)
  {
    _pInput->Unbind();
  }

 /* if (m_pRenderTarget)
  {
    m_pRenderTarget->Unbind();
  }
  else
  {
    Engine::GetInstance()->GetWindow()->UnbindDefaultRenderTarget();
  }  */
}

