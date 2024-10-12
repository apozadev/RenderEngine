#pragma once

enum TextureUsage : uint32_t
{
  NONE              = 0u,
  SHADER_RESOURCE   = 1u,
  COLOR_TARGET      = 2u,
  DEPTH_TARGET      = 4u
};