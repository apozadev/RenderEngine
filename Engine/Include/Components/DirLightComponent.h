#pragma once

#include "Core/Component.h"
#include "Graphics/DirLight.h"
#include "Memory/PtrTypes.h"

#include <glm/vec3.hpp>

class DirLightComponent : public Component
{

public:
  DirLightComponent();
  ~DirLightComponent();

  void SetColor(glm::vec3 _vColor) { m_oDirLight.m_vColor = _vColor; }

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private:

  DirLight m_oDirLight;

};