#pragma once

#include <string>

#include "Graphics/ResourceFrequency.h"
#include "Graphics/PipelineStage.h"

struct ResourceBindInfo
{
  PipelineStage m_eStage;
  ResourceFrequency m_eLevel;
  std::string m_sName;
};
