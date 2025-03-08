#pragma once

#include "Reflection/ReflectionMacros.h"

enum class DepthCompareOp
{  
  LESS,
  EQUAL,
  LESS_OR_EQUAL,
  GREATER,
  NOT_EQUAL,
  GREATER_OR_EQUAL,
  ALWAYS,
  NEVER
};

DECLARE_REFLECTION_ENUM(DepthCompareOp)
