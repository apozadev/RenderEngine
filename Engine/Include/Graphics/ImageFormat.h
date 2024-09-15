#pragma once

enum class ImageFormat
{
  D32,
  R8G8B8,
  R8G8B8A8,
  COUNT
};

inline size_t GetImageFormatSize(ImageFormat _eFormat)
{
  switch (_eFormat)
  {
  case ImageFormat::D32:
    return 4;
  case ImageFormat::R8G8B8:
    return 3;    
  case ImageFormat::R8G8B8A8:
    return 4;
  default:
    break;
  }

  return 0;
}