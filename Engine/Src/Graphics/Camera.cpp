#include <glm/ext/matrix_clip_space.hpp>

#include "Graphics/Camera.h"

#include <memory>

#include "Core/Engine.h"
#include "Graphics/Window.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderPipelineConfig.h"

#include "Memory/Factory.h"

#include "Math/Transform.h"

void Camera::Configure(const std::string&  _sRenderPipelineId, bool _bOrtho, float _fAspect)
{
  m_bOrtho = _bOrtho;

  m_fAspect = _fAspect;

  m_pAPICamera = api::CreateAPICamera();

  m_pCBuffer = Factory::Create<ConstantBuffer<GlobalBufferData>>();
  m_pCBuffer->Configure();

  m_pSubState = api::CreateAPIRenderSubState(ResourceFrequency::GLOBAL);  

  m_sRenderPipelineId = _sRenderPipelineId;
}

/*
Camera::Camera(Camera&& _oOther)
  : m_pAPICamera(std::move(_oOther.m_pAPICamera))
  , m_pCBuffer(std::move(_oOther.m_pCBuffer))
  , m_pSubState(std::move(_oOther.m_pSubState))
  , m_sRenderPipelineId(std::move(_oOther.m_sRenderPipelineId))
{
}*/

Camera::~Camera()
{
  api::DestroyAPICamera(m_pAPICamera);
}

void Camera::UpdateTransform(const Transform& _oParentTransform)
{
  GlobalBufferData oData{};  
 
  oData.m_mViewInv = _oParentTransform.GetMatrix();
  oData.m_mView = glm::inverse(oData.m_mViewInv);
  oData.m_mProj = GetProjMatrix();
  oData.m_mViewProj = oData.m_mProj * oData.m_mView;
  
  m_pCBuffer->SetData(&oData);

  m_pCBuffer->Update();
}

void Camera::PreRenderSetup()
{
  api::BeginSubStateSetup(m_pSubState);
  m_pCBuffer->SetupRenderSubState("GlobalBuffer", STAGE_VERTEX | STAGE_PIXEL, ResourceFrequency::GLOBAL);
  Renderer::GetInstance()->SetupSubStateShadowMaps(ResourceFrequency::GLOBAL);
  //Renderer::GetInstance()->GetEnvMap()->SetupRenderSubState("Skybox", STAGE_PIXEL, ResourceFrequency::GLOBAL);
  Renderer::GetInstance()->GetEnvMap2()->GetColorTextures()[0]->SetupRenderSubState("Skybox", STAGE_PIXEL, ResourceFrequency::GLOBAL);
  api::EndSubStateSetup(ResourceFrequency::GLOBAL);
}

void Camera::Bind() const
{  
  api::BindAPICamera(m_pAPICamera);
  api::BindAPIRenderSubState(m_pSubState, ResourceFrequency::GLOBAL);
  m_pCBuffer->Bind();  
}

uint64_t Camera::GetKey() const
{
  return 0u;
}

const std::string& Camera::GetRenderPipelineId() const
{
  return m_sRenderPipelineId;
}

glm::mat4x4 Camera::GetProjMatrix()
{
  const Window* pWindow = Engine::GetInstance()->GetWindow();

  float fAspect = m_fAspect;

  if (fAspect < 0.f)
  {
    fAspect = (float)pWindow->GetWidth() / pWindow->GetHeight();
  }

  return m_bOrtho ? glm::orthoZO(-5.f, 5.f, -5.f, 5.f, 0.1f, 100.f) : glm::perspective(45.f, fAspect, m_fNear, m_fFar);
}
