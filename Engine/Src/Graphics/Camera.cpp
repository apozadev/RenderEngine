#include <glm/ext/matrix_clip_space.hpp>

#include "Graphics/Camera.h"

#include <memory>

#include "Core/Engine.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderPipelineConfig.h"

#include "Math/Transform.h"

struct GlobalBufferData
{
  glm::mat4 m_mViewProj;
};

class Camera::Impl
{

public:

  api::APICamera* m_pAPICamera;

  api::APIRenderSubState* m_pSubState;

  std::unique_ptr<ConstantBuffer<GlobalBufferData>> m_pCBuffer;

  std::string m_sRenderPipelineId;

  Impl(const std::string& _sRenderPipelineId)    
  {        
    m_pAPICamera = api::CreateAPICamera();

    m_pCBuffer = std::make_unique<ConstantBuffer<GlobalBufferData>>(0, PipelineStage::VERTEX);

    m_pSubState = api::CreateAPIRenderSubState(ResourceFrequency::GLOBAL);

    api::BeginSubStateSetup(m_pSubState);
    m_pCBuffer->SetupRenderSubState(ResourceFrequency::GLOBAL);
    api::EndSubStateSetup(ResourceFrequency::GLOBAL);

    m_sRenderPipelineId = _sRenderPipelineId;
  }
  ~Impl()
  {
    api::DestroyAPICamera(m_pAPICamera);
  }
};

Camera::Camera(const std::string&  _sRenderPipelineId)
  : m_pImpl(std::make_unique<Impl>(_sRenderPipelineId))
{

}

Camera::Camera(Camera&& _rCamera)
  : m_pImpl(std::move(_rCamera.m_pImpl))
{

}

Camera::~Camera()
{

}

void Camera::UpdateTransform(const Transform& _oParentTransform)
{
  GlobalBufferData oData{};

  const Window* pWindow = Engine::GetInstance()->GetWindow();

  glm::mat4x4 mView = glm::inverse(_oParentTransform.GetMatrix());
  oData.m_mViewProj = glm::perspective(45.f, (float)pWindow->GetWidth() / pWindow->GetHeight(), m_fNear, m_fFar) * mView;
  
  m_pImpl->m_pCBuffer->SetData(&oData);

  m_pImpl->m_pCBuffer->Update();
}

void Camera::Bind() const
{
  api::BindAPICamera(m_pImpl->m_pAPICamera);
  api::BindAPIRenderSubState(m_pImpl->m_pSubState, ResourceFrequency::GLOBAL);
  m_pImpl->m_pCBuffer->Bind();  
}

uint64_t Camera::GetKey() const
{
  return 0u;
}

const std::string& Camera::GetRenderPipelineId() const
{
  return m_pImpl->m_sRenderPipelineId;
}