#include "Core/ComponentDesc.h"

#include "Reflection/ReflectionImplMacros.h"

REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(ComponentDesc)
REFLECT_STRUCT_END(ComponentDesc)

IMPLEMENT_REFLECTION_POINTER(ComponentDesc)

IMPLEMENT_REFLECTION_VECTOR(owner_ptr<ComponentDesc>)
