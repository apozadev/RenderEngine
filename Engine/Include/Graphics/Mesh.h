#pragma once

#include <vector>
#include <memory>

#include "Math/Transform.h"
#include "Graphics/Vertex.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Core/PooledObject.h"

class Window;

class MaterialInstance;

struct MeshConstant
{
  glm::mat4 m_mModel;
  glm::mat4 m_mNormal;
};

class Mesh : public PooledObject<Mesh, 256>
{

  friend class RenderStep;

public:    

  using PooledObject<Mesh, 256>::PooledObject;
  
  ~Mesh();

  void Initialize(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices);

  void UpdateTransform(const Transform& _oParentTransform);  

private: 

  void Draw() const;  

private:

  api::APIMesh* m_pAPIMesh;

  glm::mat4 m_mLocalTransform;

  uint32_t m_uVertexCount;

  uint32_t m_uIndexCount;

  MeshConstant m_oConstant;

};