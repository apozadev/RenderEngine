#pragma once

#include <memory>
#include <vector>

#include "Graphics/RenderStep.h"

class GeometryRenderStep : public RenderStep
{
public:

  GeometryRenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget, bool _bOrderTranslucent);  

  void SubmitJob(Job&& _rJob) override;  

  bool IsOrderTranslucent() const { return m_bOrderTranslucent; }

protected:

  void Prepare(const Camera* _pCamera, const Transform* _pViewTransform) override;

  const Pass* GetFirstPass() const override;

  void ExecuteInternal(const Camera* _pCamera, const Transform* _pViewTransform) override;

private:

  bool m_bOrderTranslucent;

  std::vector<Job> m_lstJobs;  

};
