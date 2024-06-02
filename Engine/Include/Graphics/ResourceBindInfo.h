#pragma once

#include "Graphics/ResourceFrequency.h"
#include "Graphics/PipelineStage.h"

struct ResourceBindInfo
{
  PipelineStage m_eStage;
  ResourceFrequency m_eLevel;
  int m_iBinding;
};
