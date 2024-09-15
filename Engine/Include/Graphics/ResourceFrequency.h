#pragma once

enum class ResourceFrequency : int
{
  GLOBAL,
  RENDER_STEP,
  MATERIAL,
  MATERIAL_INSTANCE,

  COUNT
};
