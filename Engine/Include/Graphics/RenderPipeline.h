#pragma once

#include <vector>
#include <string>

#include "RenderStep.h"
#include "Job.h"

class RenderTarget;

struct RenderPipelineConfig;

class Window;

class RenderPipeline
{
public:

	RenderPipeline(const Window* _pWindow, const RenderPipelineConfig* _pConfig);
	~RenderPipeline();

	void Execute(const std::vector<Job>& _lstJobs, const Camera* _pCamera) const;

private:

	std::vector<RenderStep> m_lstRenderSteps;
	std::vector<RenderTarget*> m_lstRenderTargets;
};