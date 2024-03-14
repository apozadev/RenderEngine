#pragma once

#include <vector>
#include <memory>

#include "Graphics\Vertex.h"

class Window;

class Mesh
{

  friend class Renderer;

  struct ConstructKey {};

public:    

  Mesh(std::vector<Vertex>& _lstVertices, Window* _pWindow, ConstructKey&&);
  ~Mesh();

private: 

  void Draw() const;
  Window* GetWindow() const;
  uint64_t GetKey() const;

  class Impl;
  Impl* m_pImpl;
};