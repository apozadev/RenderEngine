#pragma once

#include <vector>
#include <string>

#include "RenderPipelineConfig.h"
#include "RenderStep.h"
#include "Job.h"

class RenderTarget;
class RenderStep;
class Window;

struct RenderPipelineConfig;


class RenderPipeline
{
public:

	RenderPipeline(std::string _sId, RenderPipelineConfig&& _rConfig);
	RenderPipeline(RenderPipeline&& _rPipeline) = default;
	~RenderPipeline();	

	const std::string& GetId() const { return m_sId; }

	RenderStep* GetRenderStep(int _iIdx) { if (_iIdx < m_lstRenderSteps.size()) return &m_lstRenderSteps[_iIdx]; return nullptr; }

	void OnWindowResize();

	void Execute(const Camera* _pCamera, const Transform* _pViewTransform);

	void Clear();

private:

	void GenerateFromConfig();

private:

	std::string m_sId;
	
	std::vector<RenderStep> m_lstRenderSteps;
	std::vector<RenderTarget*> m_lstRenderTargets;

	RenderPipelineConfig m_oConfig;
};