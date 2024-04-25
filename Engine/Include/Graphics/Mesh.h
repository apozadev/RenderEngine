#pragma once

#include <vector>
#include <memory>

#include "Graphics/Vertex.h"
#include "Math/Transform.h"

class Window;

class MaterialInstance;

class Mesh
{

  friend class Renderer;

public:    

  Mesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, MaterialInstance* _pMaterial, Window* _pWindow);
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