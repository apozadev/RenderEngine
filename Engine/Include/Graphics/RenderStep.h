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

  RenderStep(const std::string& _sId, std::vector<Texture2D*>&& _lstInputs, const RenderTarget* _pRenderTarget);

  virtual ~RenderStep();

  const std::string& GetId() const { return m_sId; }

  void Setup();

  virtual void SubmitJob(Job&& _rJob) {};
  virtual void SubmitJobs(const std::vector<Job>& _rJob) {};

  void Execute(const Camera* _pCamera, const Transform* _pViewTransform, bool _bDoBindRT);

  const RenderTarget* GetRenderTarget() const;  

protected:

  virtual void SetupInternal();

  virtual void Prepare(const Camera* _pCamera, const Transform* _pViewTransform) {};

  void Bind(bool _bDoBindRT, const Pass* _pPass) const;

  void Unbind() const;

  virtual const Pass* GetFirstPass() const = 0;

  virtual void ExecuteInternal(const Camera* _pCamera, const Transform* _pViewTransform) = 0;
  
private:  

  std::string m_sId;

  std::vector<Texture2D*> m_lstInputs;

  const RenderTarget* m_pRenderTarget;

  api::APIRenderSubState* m_pAPIRenderSubState;

};
