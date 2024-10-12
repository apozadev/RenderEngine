#pragma once

#include <memory>
#include <vector>

class Window;
class RenderTarget;
class Transform;
class Camera;
struct Job;

class RenderStep
{
public:

  RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget);

  RenderStep(RenderStep&& _rOther);

  ~RenderStep();

  void SubmitJob(Job&& _rJob);

  void Execute(const Camera* _pCamera, const Transform* _pViewTransform) const;

  void Clear();

  const RenderTarget* GetRenderTarget() const;

private:


  void Bind(const Window* _pWindow) const;

  void Unbind() const;
  
private:

  class Impl;

  std::unique_ptr<Impl> m_pImpl;  
};
