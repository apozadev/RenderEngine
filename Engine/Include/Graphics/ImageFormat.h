#pragma once

#include "Reflection/ReflectionMacros.h"

enum class ImageFormat
{
  R32,
  R8G8B8,  
  R16G16B16,
  R32G32B32,  
  R8G8B8_SRGB,
  R8G8B8A8,  
  R8G8B8A8_SRGB,
  R32G32B32A32,
  COUNT
};

inline size_t GetImageFormatSize(ImageFormat _eFormat)
{
  switch (_eFormat)
  {
  case ImageFormat::R32:
    return 4;
  case ImageFormat::R8G8B8:
  case ImageFormat::R8G8B8_SRGB:
    return 3;      
  case ImageFormat::R8G8B8A8:
  case ImageFormat::R8G8B8A8_SRGB:
    return 4;  
  case ImageFormat::R16G16B16:
    return 6;
  case ImageFormat::R32G32B32:
    return 12;
  case ImageFormat::R32G32B32A32:
    return 16;
  default:
    break;
  }

  return 0;
}

DECLARE_REFLECTION_ENUM(ImageFormat)
