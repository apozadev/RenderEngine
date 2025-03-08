#pragma once

#include <memory>
#include <vector>

#include "Graphics/RenderStep.h"
#include "Graphics/Texture2D.h"

class GeometryRenderStep : public RenderStep
{
public:

  GeometryRenderStep(std::vector<Texture2D*>&& _lstInputs, const RenderTarget* _pRenderTarget, bool _bOrderTranslucent, bool _bUseShadowMaps = true);  

  void SubmitJob(Job&& _rJob) override;  
  void SubmitJobs(const std::vector<Job>& _rJob) override;

  bool IsOrderTranslucent() const { return m_bOrderTranslucent; }

protected:

  void SetupInternal() override;

  void Prepare(const Camera* _pCamera, const Transform* _pViewTransform) override;

  const Pass* GetFirstPass() const override;

  void ExecuteInternal(const Camera* _pCamera, const Transform* _pViewTransform) override;

private:

  bool m_bOrderTranslucent;
  bool m_bUseShadowMaps;

  std::vector<Job> m_lstJobs;  

};
