#include "Core/SceneDesc.h"

#include "Reflection/ReflectionImplMacros.h"

REFLECT_STRUCT_BASE_BEGIN(EntityDesc)
REFLECT_STRUCT_MEMBER(vPos)
REFLECT_STRUCT_MEMBER(vRot)
REFLECT_STRUCT_MEMBER(vScl)
REFLECT_STRUCT_MEMBER(Components)
REFLECT_STRUCT_END(EntityDesc)

REFLECT_STRUCT_BASE_BEGIN(SceneDesc)
REFLECT_STRUCT_MEMBER(Entities)
REFLECT_STRUCT_END(SceneDesc)

IMPLEMENT_REFLECTION_POINTER(EntityDesc)

IMPLEMENT_REFLECTION_VECTOR(owner_ptr<EntityDesc>)
