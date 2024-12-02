#include <glm/ext/matrix_clip_space.hpp>

#include "Graphics/Camera.h"

#include <memory>

#include "Core/Engine.h"
#include "Graphics/Window.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderPipelineConfig.h"

#include "Memory/Factory.h"

#include "Math/Transform.h"

Camera::Camera(const std::string&  _sRenderPipelineId)
{
  m_pAPICamera = api::CreateAPICamera();

  m_pCBuffer = Factory::Create<ConstantBuffer<GlobalBufferData>>();
  m_pCBuffer->Configure(0, PipelineStage::VERTEX);

  m_pSubState = api::CreateAPIRenderSubState(ResourceFrequency::GLOBAL);

  api::BeginSubStateSetup(m_pSubState);
  m_pCBuffer->SetupRenderSubState(ResourceFrequency::GLOBAL);
  api::EndSubStateSetup(ResourceFrequency::GLOBAL);

  m_sRenderPipelineId = _sRenderPipelineId;
}

Camera::Camera(Camera&& _oOther)
  : m_pAPICamera(std::move(_oOther.m_pAPICamera))
  , m_pCBuffer(std::move(_oOther.m_pCBuffer))
  , m_pSubState(std::move(_oOther.m_pSubState))
  , m_sRenderPipelineId(std::move(_oOther.m_sRenderPipelineId))
{
}

Camera::~Camera()
{
  api::DestroyAPICamera(m_pAPICamera);
}

void Camera::UpdateTransform(const Transform& _oParentTransform)
{
  GlobalBufferData oData{};

  const Window* pWindow = Engine::GetInstance()->GetWindow();

  glm::mat4x4 mView = glm::inverse(_oParentTransform.GetMatrix());
  oData.m_mViewProj = glm::perspective(45.f, (float)pWindow->GetWidth() / pWindow->GetHeight(), m_fNear, m_fFar) * mView;
  
  m_pCBuffer->SetData(&oData);

  m_pCBuffer->Update();
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