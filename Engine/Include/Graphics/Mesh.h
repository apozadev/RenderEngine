#pragma once

#include <vector>
#include <memory>

#include "Math/Transform.h"
#include "Graphics/Vertex.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Core/BaseObject.h"

class Window;

class MaterialInstance;

struct MeshConstant
{
  glm::mat4 m_mModel;
  glm::mat4 m_mNormal;
};

class Mesh : public BaseObject
{

  friend class GeometryRenderStep;

public:      
  
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