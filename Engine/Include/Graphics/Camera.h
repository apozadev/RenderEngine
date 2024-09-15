#pragma once

#include <vector>
#include <memory>

class Window;
class Transform;
class RenderPipeline;
struct RenderPipelineConfig;

class Camera
{

  friend class Renderer;

public:

  Camera(Window* _pWindow, const RenderPipelineConfig* _pPipelineConfig);
  Camera(Camera&& _rCamera);
  ~Camera();  

  void UpdateTransform(const Transform& _oParentTransform);

  void Bind() const;

  Window* GetWindow() const;

  uint64_t GetKey() const;

  const RenderPipeline* GetRenderPipeline() const;

  float m_fNear = 0.1f;
  float m_fFar = 100.f;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;  
};