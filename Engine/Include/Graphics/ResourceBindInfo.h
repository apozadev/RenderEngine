#pragma once

#include <string>

#include "Graphics/ResourceFrequency.h"
#include "Graphics/PipelineStage.h"

struct ResourceBindInfo
{
  PipelineStageFlags m_uStageFlags;
  ResourceFrequency m_eLevel;
  std::string m_sName;
};
