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
#include "Graphics/RenderPipelineConfig.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderKey.h"
#include "Graphics/RenderStep.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/DirLight.h"
#include "Graphics/GeometryRenderStep.h"
#include "Memory/Factory.h"

#include "Core/Exception.h"

#include "GLFW/glfw3.h"

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

void Renderer::Initialize()
{  
   
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  api::InitializeAPI();

}

void Renderer::InitializePostWindow()
{
  m_pLightCBuff = Factory::Create<ConstantBuffer<LightData>>();
  m_pLightCBuff->Configure();

  m_pLightCBuff->GetData()->m_uNumLights = 0u;
}

void Renderer::ShutDownPreWindow()
{
  glfwTerminate();

  m_lstCamViews.clear();

  m_lstRenderPipelines.clear();

  m_pLightCBuff.reset();
}

void Renderer::ShutDownPostWindow()
{  
  api::ShutDownAPI();
}

void Renderer::AddRenderPipeline(RenderPipelineConfig&& _pPipelineCofig)
{
  std::string sId = _pPipelineCofig.m_sId;
  m_lstRenderPipelines.push_back(std::move(RenderPipeline(sId, std::move(_pPipelineCofig))));
}

void Renderer::Setup() 
{
  for (RenderPipeline& rPipeline : m_lstRenderPipelines)
  {
    rPipeline.Setup();
  }
}

void Renderer::SubmitCamera(Camera* _pCamera, const Transform* _pTransform)
{
  m_lstCamViews.push_back({ _pCamera, _pTransform });
}

void Renderer::SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform, bool _bCastShadow)
{    
  for (const owner_ptr<Pass>& pPass : _pMaterial->GetMaterial()->GetPasses())
  {
    RenderPipeline* pPipeline = GetRenderPipeline(pPass->GetRenderPipelineId());
    RenderStep* pRenderStep = pPipeline ? pPipeline->GetRenderStep(pPass->GetRenderStepIdx()) : nullptr;

    if (pRenderStep)
    {
      pRenderStep->SubmitJob({ _pMesh, _pMaterial, pPass.get(), _pTransform, 0u });
    }    
  }
  if (_bCastShadow)
  {
    m_lstShadowJobs.push_back({ _pMesh, nullptr, nullptr, _pTransform, 0u });
  }
}

void Renderer::SubmitDirLight(const glm::vec3& _vColor, Camera* _pCamera, const Transform* _pTransform, const RenderTarget* _pShadowMap, const Pass* _pPass)
{
  if (m_pLightCBuff->GetData()->m_uNumLights < MAX_LIGHTS)
  {
    DirLightData oData{};    
    oData.m_vColor = glm::vec4{ _vColor, 1.f };
    oData.m_vDir = glm::vec4{ _pTransform->GetFront(), 1.f };

    m_pLightCBuff->GetData()->m_aLights[m_pLightCBuff->GetData()->m_uNumLights] = oData;

    glm::mat4& mLightView = m_pLightCBuff->GetData()->m_aLightViews[m_pLightCBuff->GetData()->m_uNumShadows];
    mLightView = _pCamera->GetProjMatrix() * glm::inverse(_pTransform->GetMatrix());

    m_lstShadowViews.push_back({ _pCamera, _pTransform, _pShadowMap ,_pPass });

    m_pLightCBuff->GetData()->m_uNumLights++;
    m_pLightCBuff->GetData()->m_uNumShadows++;
  }
}

void Renderer::OnWindowResize()
{
  for (RenderPipeline& rPipeline : m_lstRenderPipelines)
  {        
    rPipeline.OnWindowResize();    
  }
}

RenderPipeline* Renderer::GetRenderPipeline(std::string _sPipelineId)
{
  for (RenderPipeline& rPipeline : m_lstRenderPipelines)
  {
    if (rPipeline.GetId() == _sPipelineId)
    {
      return &rPipeline;
    }
  }

  return nullptr;
}

void Renderer::SetupSubStateLightCBuffers(ResourceFrequency _eFrequency)
{
  m_pLightCBuff->SetupRenderSubState("LightBuffer", PipelineStage::PIXEL, _eFrequency);
}

void Renderer::SetupSubStateShadowMaps(ResourceFrequency _eFrequency)
{
  static const char aNames[4][11] = { "ShadowMap0", "ShadowMap1", "ShadowMap2", "ShadowMap3" };
  for (unsigned int i = 0u; i < m_lstShadowViews.size(); i++)
  {
    const RenderTarget* pShadowMap = m_lstShadowViews[i].m_pShadowMap;
    pShadowMap->GetDepthStencilTexture()->SetupRenderSubState(aNames[i], PipelineStage::PIXEL, _eFrequency);
  }
}

void Renderer::Draw()
{    
  
  bool bNeedsResize = Engine::GetInstance()->GetWindow()->BeginDraw() != 0;

  if (bNeedsResize)
  {
    OnWindowResize();    
  }
  else
  {    

    for (const ShadowView& rShadowView : m_lstShadowViews)
    {
      rShadowView.m_pCamera->PreRenderSetup();
    }

    for (CamView& rCamView : m_lstCamViews)
    {
      rCamView.m_pCamera->PreRenderSetup();
    }

    std::vector<owner_ptr<GeometryRenderStep>> lstShadowSteps(m_lstShadowViews.size());

    for (const ShadowView& rShadowView : m_lstShadowViews)
    {
      rShadowView.m_pShadowMap->Clear();

      owner_ptr<GeometryRenderStep> pShadowStep = Factory::Create<GeometryRenderStep>(std::vector<RenderTarget*>(), rShadowView.m_pShadowMap, false, false);

      for (Job& rJob : m_lstShadowJobs)
      {
        Job rNewJob = rJob;
        rNewJob.m_pPass = rShadowView.m_pPass;
        pShadowStep->SubmitJob(std::move(rNewJob));
      }
      
      pShadowStep->Setup();

      pShadowStep->Execute(rShadowView.m_pCamera, rShadowView.m_pTransform);

      rShadowView.m_pShadowMap->Unbind();

      lstShadowSteps.push_back(std::move(pShadowStep));
    }


    m_pLightCBuff->Update();

    m_pLightCBuff->Bind();

    for (const ShadowView& rShadowView : m_lstShadowViews)
    {      
      rShadowView.m_pShadowMap->GetDepthStencilTexture()->Bind();
    }

    for (CamView& rCamView : m_lstCamViews)
    {

      // Execute pipeline            
      if (RenderPipeline* pPipeline = GetRenderPipeline(rCamView.m_pCamera->GetRenderPipelineId()))
      {
        pPipeline->Execute(rCamView.m_pCamera, rCamView.m_pTransform);
      }
      else
      {
        THROW_GENERIC_EXCEPTION("Camera RenderPipeline Id is not valid")
      }

    }    

    for (const ShadowView& rShadowView : m_lstShadowViews)
    {
      rShadowView.m_pShadowMap->GetDepthStencilTexture()->Unbind();
    }

    Engine::GetInstance()->GetWindow()->EndDraw();
  }
  

  m_lstCamViews.clear();
  m_lstShadowViews.clear();
  m_lstShadowJobs.clear();
  m_pLightCBuff->GetData()->m_uNumLights = 0u;
  m_pLightCBuff->GetData()->m_uNumShadows = 0u;
}
