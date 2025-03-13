#pragma once

enum PipelineStage : unsigned int
{
  STAGE_NONE    = 0,
  STAGE_VERTEX  = 1 << 0,
  STAGE_PIXEL   = 1 << 1
};

typedef std::underlying_type<PipelineStage>::type PipelineStageFlags;
