#pragma once

#include <vector>
#include <string>

#include "RenderPipelineConfig.h"
#include "RenderStep.h"
#include "Job.h"

class RenderTarget;

struct RenderPipelineConfig;

class Window;

class RenderPipeline
{
public:

	RenderPipeline(const Window* _pWindow, RenderPipelineConfig&& _rConfig);
	RenderPipeline(RenderPipeline&& _rPipeline) = default;
	~RenderPipeline();

	const Window* GetOwnerWindow() const { return m_pWindow; }

	void OnWindowResize();

	void Execute(const std::vector<Job>& _lstJobs, const Camera* _pCamera) const;

private:

	void GenerateFromConfig();

private:
	
	std::vector<RenderStep> m_lstRenderSteps;
	std::vector<RenderTarget*> m_lstRenderTargets;

	RenderPipelineConfig m_oConfig;

	const Window* m_pWindow;
};