#pragma once

#include "Core/Component.h"
#include "Core/ComponentDesc.h"
#include "Graphics/Camera.h"
#include "Memory/PtrTypes.h"

class Window;

class CameraComponent : public Component
{
public:

  REFLECT()

  CameraComponent();       

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private:  

  owner_ptr<Camera> m_pCamera;  

  float m_fLastMousePosX;
  float m_fLastMousePosY;

  float m_fYaw;
  float m_fPitch;

};

DECLARE_COMPONENT_DESC_BEGIN(CameraComponentDesc,CameraComponent)
float m_fFov;
float m_fNear;
float m_fFar;
DECLARE_COMPONENT_DESC_END(CameraComponentDesc)
