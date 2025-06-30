#pragma once

enum TextureUsage : uint32_t
{
  NONE              = 0u,
  SHADER_RESOURCE   = 1u << 0u,
  COLOR_TARGET      = 1u << 1u,
  DEPTH_TARGET      = 1u << 2u,
  TRANSFER_SRC      = 1u << 3u,
  TRANSFER_DST      = 1u << 4u
};