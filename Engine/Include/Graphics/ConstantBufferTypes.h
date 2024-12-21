#pragma once

enum class ConstantBufferType
{
  NONE,
  SCALAR,
  VEC2,
  VEC3,
  VEC4,
  MAT3,
  MAT4
};

size_t GetConstantBufferTypeSize(ConstantBufferType _eType);