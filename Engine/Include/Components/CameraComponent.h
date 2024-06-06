#pragma once

#include "Core/Component.h"

#include "Graphics/Camera.h"

class Window;

class CameraComponent : public Component
{

public:
  CameraComponent(Window* _pWindow);   

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private:  

  Camera m_oCamera;

  float m_fFov;
  float m_fNear;
  float m_fFar;
};