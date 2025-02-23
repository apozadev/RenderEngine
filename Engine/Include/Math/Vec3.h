
#include "Core/BaseObject.h"
#include "Reflection/ReflectionMacros.h"

struct Vec3 : public BaseObject
{
  REFLECT_BASE()

  float x, y, z;
};
