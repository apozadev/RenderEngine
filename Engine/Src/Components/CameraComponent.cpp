#include <glm/ext/matrix_clip_space.hpp>

#include "Core/Entity.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Components/CameraComponent.h"

CameraComponent::CameraComponent(Window* _pWindow)
  : m_oCamera(_pWindow)
{

}

void CameraComponent::Start() 
{
  m_fFov = 90.f;
  m_fNear = 0.1F;
  m_fFar = 100.f;
}

void CameraComponent::PreTransformUpdate(float _fTimeStep)
{
  constexpr float fSpeed = 5.f;

  glm::vec3 vTranslation(0.f);

  if (m_oCamera.GetWindow()->IsKeyPressed('a'))
  {
    vTranslation -= fSpeed * _fTimeStep;
  }

  if (m_oCamera.GetWindow()->IsKeyPressed('d'))
  {
    vTranslation += fSpeed * _fTimeStep;
  }

  Transform& oTr = m_pEntity->GetMutableLocalTransform();
  oTr.SetPos(oTr.GetPos() + vTranslation);
}

void CameraComponent::Update(float _fTimeStep) 
{  
  m_oCamera.UpdateTransform(m_pEntity->GetGlobalTransform());
  Renderer::GetInstance()->SubmitCamera(&m_oCamera);
}