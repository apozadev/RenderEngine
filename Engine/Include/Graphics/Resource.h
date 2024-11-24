#pragma once

#include "Core/PooledObject.h"
#include "Graphics/ResourceFrequency.h"
#include "Graphics/PipelineStage.h"

class Resource 
{
public:

  Resource(int _iBinding, PipelineStage _eStage)
    : m_iBinding(_iBinding), m_eStage(_eStage)
  {}

  virtual ~Resource() = default;

  virtual void Bind() const = 0;

  virtual void SetupRenderSubState(ResourceFrequency _eFrequency) const = 0;
  

  int m_iBinding;
  PipelineStage m_eStage;
};