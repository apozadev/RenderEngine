#include "Graphics/RenderStep.h"

#include <vector>

#include "Graphics/RenderTarget.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Window.h"

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

  std::vector<RenderTarget*> m_lstInputs;

  const RenderTarget* m_pRenderTarget;

  api::APIRenderSubState* m_pAPIRenderSubState;

};

RenderStep::RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget)
{
  m_pImpl = std::make_unique<Impl>(std::move(_lstInputs), _pRenderTarget);
}

RenderStep::RenderStep(RenderStep&& _rOther)
  : m_pImpl(std::move(_rOther.m_pImpl))
{
}

RenderStep::~RenderStep()
{
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
