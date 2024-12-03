#include <glm/ext/matrix_clip_space.hpp>

#include "Core/Engine.h"
#include "Core/Entity.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Components/CameraComponent.h"
#include "Math/Utils.h"
#include "Memory/Factory.h"

CameraComponent::CameraComponent()  
{  
  m_pCamera = Factory::Create<Camera>();
  m_pCamera->Configure("TEST");
}

void CameraComponent::Start() 
{
  m_fFov = 90.f;
  m_fNear = 0.1F;
  m_fFar = 100.f;

  const Window* pWindow = Engine::GetInstance()->GetWindow();

  double dPosX, dPosY;
  pWindow->GetMousePos(dPosX, dPosY);
  m_fLastMousePosX = static_cast<float>(dPosX);
  m_fLastMousePosY = static_cast<float>(dPosY);

  glm::vec3 vRot = glm::eulerAngles(m_pEntity->GetGlobalTransform().GetRot());
  m_fPitch = vRot.x;
  m_fYaw = vRot.y;

  /*glm::vec3 vRotLocal = glm::eulerAngles(m_pEntity->GetGlobalTransform().GetRot());
  vRotLocal.z = _PI;
  m_pEntity->GetMutableLocalTransform().SetRot(glm::quat(vRotLocal));*/
}

void CameraComponent::PreTransformUpdate(float _fTimeStep)
{
  constexpr float fMoveSpeed = 10.f;
  constexpr float fRotSpeed = 0.3f;  

  const Window* pWindow = Engine::GetInstance()->GetWindow();

  Transform& rTr = m_pEntity->GetMutableLocalTransform();  

  double dMousePosX, dMousePosY;

  pWindow->GetMousePos(dMousePosX, dMousePosY);

  glm::vec3 vTranslation(0.f);

  if (pWindow->IsKeyPressed('a'))
  {
    vTranslation -= rTr.GetRight() * fMoveSpeed * _fTimeStep;
  }

  if (pWindow->IsKeyPressed('d'))
  {
    vTranslation += rTr.GetRight() * fMoveSpeed * _fTimeStep;
  }

  if (pWindow->IsKeyPressed('w'))
  {
    vTranslation -= rTr.GetFront() * fMoveSpeed * _fTimeStep;
  }

  if (pWindow->IsKeyPressed('s'))
  {
    vTranslation += rTr.GetFront() * fMoveSpeed * _fTimeStep;
  }

  if (pWindow->IsKeyPressed('q'))
  {
    vTranslation -= rTr.GetUp() * fMoveSpeed * _fTimeStep;
  }

  if (pWindow->IsKeyPressed('e'))
  {
    vTranslation += rTr.GetUp() * fMoveSpeed * _fTimeStep;
  }

  rTr.SetPos(rTr.GetPos() + vTranslation);

  if (pWindow->IsMousePressed(0))
  {        
    float fDeltaX = dMousePosX - static_cast<float>(m_fLastMousePosX);
    float fDeltaY = dMousePosY - static_cast<float>(m_fLastMousePosY);            

    m_fYaw -= fRotSpeed * fDeltaY * _fTimeStep;    
    m_fPitch -= fRotSpeed * fDeltaX * _fTimeStep;    

    //rTr.SetRot(glm::quat(glm::vec3(m_fYaw, m_fPitch, _PI)));
    rTr.SetRot(glm::quat(glm::vec3(m_fYaw, m_fPitch, 0.f)));
  }  

  m_fLastMousePosX = static_cast<float>(dMousePosX);
  m_fLastMousePosY = static_cast<float>(dMousePosY);

}

void CameraComponent::Update(float _fTimeStep) 
{  
  m_pCamera->UpdateTransform(m_pEntity->GetGlobalTransform());
  Renderer::GetInstance()->SubmitCamera(m_pCamera.get(), &m_pEntity->GetGlobalTransform());
}