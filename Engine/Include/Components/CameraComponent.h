#pragma once

#include "Core/Component.h"
#include "Graphics/Camera.h"

class Window;

class CameraComponent : public Component
{

public:
  CameraComponent();   
  ~CameraComponent();

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private:  

  std::unique_ptr<Camera> m_pCamera;

  float m_fFov;
  float m_fNear;
  float m_fFar;

  float m_fLastMousePosX;
  float m_fLastMousePosY;

  float m_fYaw;
  float m_fPitch;

};