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

  RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget, bool _bOrderTranslucent);

  RenderStep(RenderStep&& _rOther);

  ~RenderStep();

  void SubmitJob(Job&& _rJob);

  void Execute(const Camera* _pCamera, const Transform* _pViewTransform) const;

  void Clear();

  const RenderTarget* GetRenderTarget() const;

  bool IsOrderTranslucent() const { return m_bOrderTranslucent; }

private:


  void Bind() const;

  void Unbind() const;
  
private:

  bool m_bOrderTranslucent;

private: 

  class Impl;

  std::unique_ptr<Impl> m_pImpl;  
};
