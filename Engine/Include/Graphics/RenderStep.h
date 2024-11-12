#pragma once

#include <memory>
#include <vector>

#include "Graphics/Job.h"
#include "Graphics/API/GraphicsAPI.h"

class Window;
class RenderTarget;
class Transform;
class Camera;

class RenderStep
{
public:

  RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget, bool _bOrderTranslucent);

  ~RenderStep();

  void SubmitJob(Job&& _rJob);

  void Execute(const Camera* _pCamera, const Transform* _pViewTransform);

  void Clear();

  const RenderTarget* GetRenderTarget() const;

  bool IsOrderTranslucent() const { return m_bOrderTranslucent; }

private:

  void Bind() const;

  void Unbind() const;
  
private:

  bool m_bOrderTranslucent;

private: 

  std::vector<Job> m_lstJobs;

  std::vector<RenderTarget*> m_lstInputs;

  const RenderTarget* m_pRenderTarget;

  api::APIRenderSubState* m_pAPIRenderSubState;
};
