#pragma once

#include "Reflection/ReflectionMacros.h"

#include "Core/BaseObject.h"

#define DECLARE_COMPONENT_DESC_BEGIN(name, component)  \
  class name : public ComponentDesc { \
    public: \
    REFLECT() \
    const reflection::TypeDescriptor* GetComponentType() const { return &component::GetReflection(); }   

#define DECLARE_COMPONENT_DESC_END(name)  \
  }; \
  DECLARE_REFLECTION_POINTER(name)  \
  DECLARE_REFLECTION_VECTOR(owner_ptr<name>)

#define IMPLEMENT_COMPONENT_DESC_BEGIN(name) REFLECT_STRUCT_BEGIN(name, ComponentDesc)
#define IMPLEMENT_COMPONENT_DESC_END(name)  \
  REFLECT_STRUCT_END(name)  \
  IMPLEMENT_REFLECTION_POINTER(name)  \
  IMPLEMENT_REFLECTION_VECTOR(name)

class ComponentDesc : public BaseObject
{
public:
  REFLECT_BASE();

  virtual const reflection::TypeDescriptor* GetComponentType() const = 0;
};

DECLARE_REFLECTION_POINTER(ComponentDesc)

DECLARE_REFLECTION_VECTOR(owner_ptr<ComponentDesc>)
