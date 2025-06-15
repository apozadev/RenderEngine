#pragma once

#include <vector>
#include <memory>

#include "Math/Transform.h"
#include "Graphics/Vertex.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Core/BaseObject.h"

#include "Reflection/ReflectionMacros.h"

class Window;

class MaterialInstance;

struct MeshConstant
{
  glm::mat4 m_mModel;
  glm::mat4 m_mNormal;
};

class Mesh : public BaseObject
{
public:      

  REFLECT_BASE()
  
  ~Mesh();

  void Initialize(const std::vector<Vertex>& _lstVertices, const std::vector<uint16_t>& _lstIndices);

  void UpdateTransform(const Transform& _oParentTransform);   

public:

  api::APIMesh* m_pAPIMesh;

  glm::mat4 m_mLocalTransform;

  uint32_t m_uVertexCount;

  uint32_t m_uIndexCount;

  MeshConstant m_oConstant;

};

DECLARE_REFLECTION_POINTER(Mesh)
