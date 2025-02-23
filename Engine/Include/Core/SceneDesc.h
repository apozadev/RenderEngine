#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Memory/PtrTypes.h"
#include "Math/Vec3.h"
#include "ComponentDesc.h"

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
