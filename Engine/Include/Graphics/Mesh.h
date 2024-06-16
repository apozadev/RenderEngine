#pragma once

#include <vector>
#include <memory>

#include "Graphics/Vertex.h"
#include "Math/Transform.h"

class Window;

class MaterialInstance;

struct MeshConstant
{
  glm::mat4 m_mModel;
  glm::mat4 m_mNormal;
};

class Mesh
{

  friend class Renderer;

public:    

  Mesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, Window* _pWindow);
  Mesh(Mesh&& _rMesh);
  ~Mesh();

  void UpdateTransform(const Transform& _oParentTransform);

private: 

  void Draw();
  Window* GetWindow() const;
  uint64_t GetKey() const;

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};