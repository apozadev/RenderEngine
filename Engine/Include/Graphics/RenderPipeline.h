#pragma once

#include <vector>
#include <string>

#include "RenderPipelineConfig.h"
#include "Job.h"

#include "Memory/PtrTypes.h"

class RenderTarget;
class RenderStep;
class Window;

struct RenderPipelineConfig;


class RenderPipeline
{
public:

	RenderPipeline(std::string _sId, RenderPipelineConfig&& _rConfig);
	RenderPipeline(RenderPipeline&& _rPipeline) = default;

	const std::string& GetId() const { return m_sId; }

	RenderStep* GetRenderStep(int _iIdx) { if (_iIdx < m_lstRenderSteps.size()) return m_lstRenderSteps[_iIdx].get(); return nullptr; }

	void OnWindowResize();

	void Execute(const Camera* _pCamera, const Transform* _pViewTransform);	

private:

	void GenerateFromConfig();

private:

	std::string m_sId;
	
	std::vector<owner_ptr<RenderStep>> m_lstRenderSteps;
	std::vector<owner_ptr<RenderTarget>> m_lstRenderTargets;

	RenderPipelineConfig m_oConfig;
};