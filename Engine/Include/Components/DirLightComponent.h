#pragma once

#include "Core/Component.h"
#include "Core/ComponentDesc.h"
#include "Graphics/DirLight.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Pass.h"
#include "Graphics/Camera.h"
#include "Math/Vec3.h"
#include "Memory/PtrTypes.h"

#include <glm/vec3.hpp>

class DirLightComponent : public Component
{

public:

  REFLECT()

  DirLightComponent();
  ~DirLightComponent();  

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private: 

  owner_ptr<RenderTarget> m_pShadowMap;
  owner_ptr<Camera> m_pCamera;
  owner_ptr<Pass> m_pShadowPass;

};

DECLARE_COMPONENT_DESC_BEGIN(DirLightComponentDesc, DirLightComponent)
Vec3 m_vColor;
DECLARE_COMPONENT_DESC_END(DirLightComponentDesc)
