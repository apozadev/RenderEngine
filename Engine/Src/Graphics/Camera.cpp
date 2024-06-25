#include <glm/ext/matrix_clip_space.hpp>

#include "Graphics/Camera.h"

#include "Graphics/API/GraphicsAPI.h"

#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"

#include "Math/Transform.h"

struct GlobalBufferData
{
  glm::mat4 m_mViewProj;
};

class Camera::Impl
{

public:

  api::APICamera* m_pAPICamera;

  std::unique_ptr<ConstantBuffer<GlobalBufferData>> m_pCBuffer;

  Window* m_pWindow;

  Impl(Window* _pWindow)
    : m_pWindow(_pWindow)
  {
    _pWindow->SetUsing();
    m_pAPICamera = api::CreateAPICamera();

    m_pCBuffer = std::make_unique<ConstantBuffer<GlobalBufferData>>(0, PipelineStage::VERTEX);

    api::BeginCameraSubStateSetup(m_pAPICamera);
    m_pCBuffer->Setup(ResourceFrequency::GLOBAL);
    api::EndCameraSubstateSetup(m_pAPICamera);
  }
  ~Impl()
  {
    api::DestroyAPICamera(m_pAPICamera);
  }
};

Camera::Camera(Window* _pWindow)
  : m_pImpl(std::make_unique<Impl>(_pWindow))
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

  glm::mat4x4 mView = glm::inverse(_oParentTransform.GetMatrix());
  oData.m_mViewProj = glm::transpose(glm::perspective(45.f, (float)m_pImpl->m_pWindow->GetWidth() / m_pImpl->m_pWindow->GetHeight(), 0.1f, 100.0f) * mView);
  
  m_pImpl->m_pCBuffer->SetData(&oData);

  m_pImpl->m_pCBuffer->Update();
}

void Camera::Bind()
{
  api::BindAPICamera(m_pImpl->m_pAPICamera);
  m_pImpl->m_pCBuffer->Bind();
}

Window* Camera::GetWindow() const
{
  return m_pImpl->m_pWindow;
}

uint64_t Camera::GetKey() const
{
  return 0u;
}