#pragma once

#include <vector>
#include <memory>

class Window;

class Transform;

class Camera
{

  friend class Renderer;

public:

  Camera(Window* _pWindow);
  Camera(Camera&& _rCamera);
  ~Camera();

  void UpdateTransform(const Transform& _oParentTransform);

  void Bind();

  Window* GetWindow() const;

  uint64_t GetKey() const;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};