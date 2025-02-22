#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Memory/PtrTypes.h"
#include "ComponentDesc.h"

struct Vec3 : public BaseObject
{
  REFLECT_BASE()

  float x, y, z;
};

class EntityDesc : public BaseObject
{
public:

  REFLECT_BASE()

  Vec3 vPos, vRot, vScl;

  std::vector<owner_ptr<ComponentDesc>> Components;
};

class SceneDesc : public BaseObject
{  
public:
  REFLECT_BASE()

  std::vector<owner_ptr<EntityDesc>> Entities;
};

DECLARE_REFLECTION_POINTER(EntityDesc)

DECLARE_REFLECTION_VECTOR(owner_ptr<EntityDesc>);
