#include <glm/ext/matrix_clip_space.hpp>

#include "Core/Entity.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Components/CameraComponent.h"
#include "Math/Utils.h"

CameraComponent::CameraComponent(Window* _pWindow)  
{

  /*RenderTargetConfig oRT = {};
  oRT.m_bHasDepthStencil */

  RenderStepConfig oStep = {};
  oStep.m_sRenderTargetId = "DEFAULT";

  RenderPipelineConfig oConfig = {};
  oConfig.m_lstSteps.push_back(std::move(oStep));

  m_pCamera = std::make_unique<Camera>(_pWindow, &oConfig);
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Start() 
{
  m_fFov = 90.f;
  m_fNear = 0.1F;
  m_fFar = 100.f;

  Window& rWindow = *m_pCamera->GetWindow();

  double dPosX, dPosY;
  rWindow.GetMousePos(dPosX, dPosY);
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

  Window& rWindow = *m_pCamera->GetWindow();

  Transform& rTr = m_pEntity->GetMutableLocalTransform();  

  double dMousePosX, dMousePosY;

  rWindow.GetMousePos(dMousePosX, dMousePosY);

  glm::vec3 vTranslation(0.f);

  if (rWindow.IsKeyPressed('a'))
  {
    vTranslation -= rTr.GetRight() * fMoveSpeed * _fTimeStep;
  }

  if (rWindow.IsKeyPressed('d'))
  {
    vTranslation += rTr.GetRight() * fMoveSpeed * _fTimeStep;
  }

  if (rWindow.IsKeyPressed('w'))
  {
    vTranslation -= rTr.GetFront() * fMoveSpeed * _fTimeStep;
  }

  if (rWindow.IsKeyPressed('s'))
  {
    vTranslation += rTr.GetFront() * fMoveSpeed * _fTimeStep;
  }

  if (rWindow.IsKeyPressed('q'))
  {
    vTranslation -= rTr.GetUp() * fMoveSpeed * _fTimeStep;
  }

  if (rWindow.IsKeyPressed('e'))
  {
    vTranslation += rTr.GetUp() * fMoveSpeed * _fTimeStep;
  }

  rTr.SetPos(rTr.GetPos() + vTranslation);

  if (rWindow.IsMousePressed(0))
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