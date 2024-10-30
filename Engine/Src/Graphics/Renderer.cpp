#include "Graphics/Renderer.h"

#include "Core/Engine.h"
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
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderKey.h"

#include "Core/Exception.h"

#include "Math/Transform.h"

#include "GLFW/glfw3.h"

struct CamView
{
  Camera* m_pCamera;
  const Transform* m_pTransform;
};

class Renderer::Impl
{  
public:

  Impl() = default;

  ~Impl() = default;
  
  std::vector<CamView> m_lstCamViews;
  std::vector<RenderPipeline> m_lstRenderPipelines;
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

  m_pImpl.reset();

  api::ShutDownAPI();
}

void Renderer::AddRenderPipeline(RenderPipelineConfig&& _pPipelineCofig)
{
  std::string sId = _pPipelineCofig.m_sId;
  m_pImpl->m_lstRenderPipelines.push_back(std::move(RenderPipeline(sId, std::move(_pPipelineCofig))));
}

void Renderer::SubmitCamera(Camera* _pCamera, const Transform* _pTransform)
{
  m_pImpl->m_lstCamViews.push_back({ _pCamera, _pTransform });
}

void Renderer::SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform)
{    
  for (const Pass& rPass : _pMaterial->GetMaterial()->GetPasses())
  {
    RenderPipeline* pPipeline = GetRenderPipeline(rPass.GetRenderPipelineId());
    RenderStep* pRenderStep = pPipeline ? pPipeline->GetRenderStep(rPass.GetRenderStepIdx()) : nullptr;

    if (pRenderStep)
    {
      pRenderStep->SubmitJob({ _pMesh, _pMaterial, &rPass, _pTransform, 0u });
    }    
  }
}

void Renderer::OnWindowResize()
{
  for (RenderPipeline& rPipeline : m_pImpl->m_lstRenderPipelines)
  {        
    rPipeline.OnWindowResize();    
  }
}

RenderPipeline* Renderer::GetRenderPipeline(std::string _sPipelineId) const
{
  for (RenderPipeline& rPipeline : m_pImpl->m_lstRenderPipelines)
  {
    if (rPipeline.GetId() == _sPipelineId)
    {
      return &rPipeline;
    }
  }

  return nullptr;
}

void Renderer::Draw()
{  

  for (CamView& rCamView : m_pImpl->m_lstCamViews)
  {                  
    if (Engine::GetInstance()->GetWindow()->BeginDraw())
    {
      OnWindowResize();
      break;
    }    

    // Execute pipeline            
    if (RenderPipeline* pPipeline = GetRenderPipeline(rCamView.m_pCamera->GetRenderPipelineId()))
    {
      pPipeline->Execute(rCamView.m_pCamera, rCamView.m_pTransform);
      pPipeline->Clear();
    }
    else
    {
      THROW_GENERIC_EXCEPTION("Camera RenderPipeline Id is not valid")
    }

  }
  
  Engine::GetInstance()->GetWindow()->EndDraw();
  

  m_pImpl->m_lstCamViews.clear();
}
