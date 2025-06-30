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


RenderStep::RenderStep(const std::string& _sId, std::vector<api::APITexture*>&& _lstInputs, api::APIRenderTarget* _pRenderTarget)
  : m_sId(_sId)
  , m_lstInputs(std::move(_lstInputs))
  , m_pRenderTarget(_pRenderTarget)
{
  m_pAPIRenderSubState = api::CreateAPIRenderSubState(ENGINE_API_WINDOW, ResourceFrequency::RENDER_STEP);
}

RenderStep::~RenderStep()
{
  api::DestroyRenderSubState(ENGINE_API_WINDOW, m_pAPIRenderSubState);
}

void RenderStep::Setup() 
{   
  api::BeginSubStateSetup(ENGINE_API_WINDOW, m_pAPIRenderSubState, ResourceFrequency::RENDER_STEP);

  SetupInternal();

  api::EndSubStateSetup(ENGINE_API_WINDOW);
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
    ResourceBindInfo oBindInfo{};
    oBindInfo.m_eLevel = ResourceFrequency::RENDER_STEP;
    oBindInfo.m_uStageFlags = STAGE_PIXEL;
    oBindInfo.m_sName = s_aNames[i];
    api::SubStateSetupTexture(ENGINE_API_WINDOW, m_lstInputs[i], oBindInfo);
  }
}

void RenderStep::Execute(const Camera* _pCamera, const Transform* _pViewTransform, bool _bDoBindRT)
{

  Prepare(_pCamera, _pViewTransform);

  const Pass* pPass = GetFirstPass();

  if (pPass != nullptr)
  {
    pPass->Bind();

    api::BindAPIRenderSubState(ENGINE_API_WINDOW, pPass->m_pAPIRenderState, _pCamera->m_pSubState, ResourceFrequency::GLOBAL);

    _pCamera->m_pCBuffer->Bind();

    Bind(_bDoBindRT, pPass);

    ExecuteInternal(_pCamera, _pViewTransform);

    Unbind();
  }
}

void RenderStep::Bind(bool _bDoBindRT, const Pass* _pPass) const
{
  if (_bDoBindRT)
  {
    if (m_pRenderTarget)
    {
      api::BindAPIRenderTarget(ENGINE_API_WINDOW, m_pRenderTarget);
    }
    else
    {      
      Engine::GetInstance()->GetWindow()->BindDefaultRenderTarget();      
    }
  }

  for (api::APITexture* pInput : m_lstInputs)
  {
    api::BindAPITexture(ENGINE_API_WINDOW, pInput);
  }  

  api::BindAPIRenderSubState(ENGINE_API_WINDOW, _pPass->m_pAPIRenderState, m_pAPIRenderSubState, ResourceFrequency::RENDER_STEP);
}

void RenderStep::Unbind() const
{
  for (api::APITexture* pInput : m_lstInputs)
  {
    api::UnbindAPITexture(ENGINE_API_WINDOW, pInput);
  }
}

