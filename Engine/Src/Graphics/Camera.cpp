#include <glm/ext/matrix_clip_space.hpp>

#include "Graphics/Camera.h"

#include <memory>

#include "Core/Engine.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderPipelineConfig.h"

#include "Memory/Factory.h"

#include "Math/Transform.h"

void Camera::Configure(const std::string&  _sRenderPipelineId, bool _bOrtho)
{
  m_bOrtho = _bOrtho;

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

  const Window* pWindow = Engine::GetInstance()->GetWindow();

  glm::mat4x4 mView = glm::inverse(_oParentTransform.GetMatrix());
  glm::mat4x4 mProj =  m_bOrtho? glm::orthoRH(-5.f, 5.f, -5.f, 5.f, 0.1f, 100.f) : glm::perspective(45.f, (float)pWindow->GetWidth() / pWindow->GetHeight(), m_fNear, m_fFar);
  oData.m_mViewProj =  mProj * mView;
  
  m_pCBuffer->SetData(&oData);

  m_pCBuffer->Update();
}

void Camera::PreRenderSetup()
{
  api::BeginSubStateSetup(m_pSubState);
  m_pCBuffer->SetupRenderSubState("GlobalBuffer", PipelineStage::VERTEX, ResourceFrequency::GLOBAL);
  Renderer::GetInstance()->SetupSubStateShadowMaps(ResourceFrequency::GLOBAL);
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