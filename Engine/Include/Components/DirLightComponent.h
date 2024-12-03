#pragma once

#include "Core/Component.h"
#include "Graphics/DirLight.h"
#include "Memory/PtrTypes.h"

class DirLightComponent : public Component
{

public:
  DirLightComponent();
  ~DirLightComponent();

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private:

  owner_ptr<DirLight> m_pDirLight;

};