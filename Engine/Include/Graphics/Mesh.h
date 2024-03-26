#pragma once

#include <vector>
#include <memory>

#include "Graphics/Vertex.h"
#include "Math/Transform.h"

class Window;

class Mesh
{

  friend class Renderer;

  struct ConstructKey {};

public:    

  Mesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, Window* _pWindow, ConstructKey&&);
  ~Mesh();

private: 

  void Draw(const Transform& _oParentTransform);
  Window* GetWindow() const;
  uint64_t GetKey() const;

  class Impl;
  Impl* m_pImpl;
};