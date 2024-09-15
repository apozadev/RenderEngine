#include "Graphics/Renderer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Job.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderPipelineConfig.h"

#include "Core/Exception.h"

#include "Math/Transform.h"

#include "GLFW/glfw3.h"

#include <algorithm>

struct CamView
{
  Camera* m_pCamera;
  const Transform* m_pTransform;
};

class Renderer::Impl
{  
public:

  Impl() {}

  ~Impl() {}

  std::vector<Job> m_lstJobs;   
  std::vector<CamView> m_lstCamViews;
  std::unordered_map<std::string, RenderPipeline> m_mapRenderPipelines;
};

Renderer::Renderer() : m_pImpl(std::make_unique<Impl>())
{

}

Renderer::~Renderer() = default;

void Renderer::Initialize()
{  
   
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  api::InitializeAPI();

}

void Renderer::ShutDown()
{

  glfwTerminate();

  api::ShutDownAPI();
}

void Renderer::AddRenderPipeline(const Window* _pWindow, RenderPipelineConfig&& _pPipelineCofig)
{
  std::string sId = _pPipelineCofig.m_sId;
  m_pImpl->m_mapRenderPipelines.emplace(sId, RenderPipeline(_pWindow, std::move(_pPipelineCofig) ));
}

void Renderer::SubmitCamera(Camera* _pCamera, const Transform* _pTransform)
{
  m_pImpl->m_lstCamViews.push_back({ _pCamera, _pTransform });
}

void Renderer::SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform)
{    
  m_pImpl->m_lstJobs.push_back({ _pMesh, _pMaterial, _pMesh->GetWindow(), _pTransform, 0u });
}

void Renderer::OnWindowResize(const Window* _pWindow)
{
  for (auto it = m_pImpl->m_mapRenderPipelines.begin(); it != m_pImpl->m_mapRenderPipelines.end(); it++)
  {
    RenderPipeline& rPipeline = it->second;
    if (rPipeline.GetOwnerWindow() == _pWindow)
    {
      rPipeline.OnWindowResize();
    }
  }
}

void Renderer::Draw()
{  

  Window* pLastWindow = nullptr;

  bool bSkipWindow = false;

  for (CamView& rCamView : m_pImpl->m_lstCamViews)
  {    

    Camera* pCamera = rCamView.m_pCamera;

    // Update key for current camera
    for (Job& rJob : m_pImpl->m_lstJobs)
    {
      rJob.UpdateRenderKey(pCamera, rCamView.m_pTransform);      
    }    

    Window* pCurrWindow = pCamera->GetWindow();

    // Bind new window?
    if (pLastWindow != pCurrWindow)
    {
      if (pLastWindow != nullptr)
      {
        pLastWindow->EndDraw();
      }
      bSkipWindow = pCurrWindow->BeginDraw() != 0;
      pLastWindow = pCurrWindow;
    }
    
    if (bSkipWindow)
    {
      break;
    }

    // Sort jobs
    std::sort(m_pImpl->m_lstJobs.begin(), m_pImpl->m_lstJobs.end(), Job::Compare);

    // Execute pipeline    

    pCamera->Bind();

    auto pIt = m_pImpl->m_mapRenderPipelines.find(pCamera->GetRenderPipelineId());
    if (pIt != m_pImpl->m_mapRenderPipelines.end())
    {
      pIt->second.Execute(m_pImpl->m_lstJobs, pCamera);
    }
    else
    {
      THROW_GENERIC_EXCEPTION("Camera RenderPipeline Id is not valid")
    }

  }

  if (!bSkipWindow && pLastWindow)
  {
    pLastWindow->EndDraw();
  }

  m_pImpl->m_lstJobs.clear();
  m_pImpl->m_lstCamViews.clear();
}
