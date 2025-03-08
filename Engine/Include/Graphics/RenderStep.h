#pragma once

#include <vector>

#include "Graphics/Job.h"
#include "Graphics/API/GraphicsAPI.h"

class Window;
class RenderTarget;
class Texture2D;
class Transform;
class Camera;
class Pass;

class RenderStep
{
public:

  RenderStep(std::vector<Texture2D*>&& _lstInputs, const RenderTarget* _pRenderTarget);

  virtual ~RenderStep();

  void Setup();

  virtual void SubmitJob(Job&& _rJob) {};
  virtual void SubmitJobs(const std::vector<Job>& _rJob) {};

  void Execute(const Camera* _pCamera, const Transform* _pViewTransform);  

  const RenderTarget* GetRenderTarget() const;  

protected:

  virtual void SetupInternal();

  virtual void Prepare(const Camera* _pCamera, const Transform* _pViewTransform) {};

  void Bind() const;

  void Unbind() const;

  virtual const Pass* GetFirstPass() const = 0;

  virtual void ExecuteInternal(const Camera* _pCamera, const Transform* _pViewTransform) = 0;
  
private:  

  std::vector<Texture2D*> m_lstInputs;

  const RenderTarget* m_pRenderTarget;

  api::APIRenderSubState* m_pAPIRenderSubState;

};
