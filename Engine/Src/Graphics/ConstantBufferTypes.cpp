#include "Graphics/ConstantBufferTypes.h"

size_t GetConstantBufferTypeSize(ConstantBufferType _eType)
{

  size_t uSize = 0u;

  switch (_eType)
  {
  case ConstantBufferType::SCALAR:
    uSize += 1u;
    break;
  case ConstantBufferType::VEC2:
    uSize += 2u;
    break;
  case ConstantBufferType::VEC3:
    uSize += 3u;
    break;
  case ConstantBufferType::VEC4:
    uSize += 4u;
    break;
  case ConstantBufferType::MAT3:
    uSize += 9u;
    break;
  case ConstantBufferType::MAT4:
    uSize += 16u;
    break;
  default:
    break;
  }

  uSize *= sizeof(float);

  return uSize;
}
