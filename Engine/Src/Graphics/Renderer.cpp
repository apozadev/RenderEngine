#include "Graphics/Renderer.h"

#include "Core/Engine.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Texture2D.h"
#include "Graphics/SamplerConfig.h"
#include "Graphics/Job.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderKey.h"
#include "Graphics/RenderStep.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/DirLight.h"
#include "Graphics/GeometryRenderStep.h"
#include "Core/ImageManager.h"
#include "Memory/Factory.h"
#include "Util/ModelLoader.h"

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

  m_pEnvMapDiff = Factory::Create<Texture2D>();

  Image aImages[6];
  aImages[0] = ImageManager::GetInstance()->LoadImage("Assets/Images/skybox/0.png");
  aImages[1] = ImageManager::GetInstance()->LoadImage("Assets/Images/skybox/1.png");
  aImages[2] = ImageManager::GetInstance()->LoadImage("Assets/Images/skybox/2.png");
  aImages[3] = ImageManager::GetInstance()->LoadImage("Assets/Images/skybox/3.png");
  aImages[4] = ImageManager::GetInstance()->LoadImage("Assets/Images/skybox/4.png");
  aImages[5] = ImageManager::GetInstance()->LoadImage("Assets/Images/skybox/5.png");

  aImages[0].m_eFormat = ImageFormat::R8G8B8A8_SRGB;

  SamplerConfig oSampler = {};
  oSampler.m_eAddressMode = TextureAddressMode::CLAMP;
  oSampler.m_eMagFilterMode = TextureFilterMode::LINEAR;
  oSampler.m_eMinFilterMode = TextureFilterMode::LINEAR;
  oSampler.m_eMipmapFilterMode = TextureFilterMode::POINT;

  m_pEnvMapDiff->ConfigureAsCubemap(&aImages[0], aImages[0].m_iWidth, aImages[0].m_iHeight, aImages[0].m_eFormat, oSampler, 1u, 1u);

  // Radiance map

  SamplerConfig oHDRISamplerConfig = {};
  oHDRISamplerConfig.m_eAddressMode = TextureAddressMode::CLAMP;
  oHDRISamplerConfig.m_eMagFilterMode = TextureFilterMode::POINT;
  oHDRISamplerConfig.m_eMinFilterMode = TextureFilterMode::POINT;
  oHDRISamplerConfig.m_eMipmapFilterMode = TextureFilterMode::POINT;

  const Image& oImage = ImageManager::GetInstance()->LoadImage("Assets/Images/hdri/graveyard_pathways_4k.hdr", false);

  owner_ptr<Texture2D> pHDRI = Factory::Create<Texture2D>();
  pHDRI->Configure(oImage, oHDRISamplerConfig, 1, 1);

  m_pEnvMapSpec = Factory::Create<RenderTarget>();
  m_pEnvMapSpec->Configure(1u, 1024, 1024, oImage.m_eFormat, false, 1, 1, true);

  Camera oCamera = {};
  oCamera.Configure("", true);  
  oCamera.UpdateTransform(Transform::GetIdentity());
  oCamera.PreRenderSetup();

  Mesh oCubeMesh = {};
  SetupCubeMesh(&oCubeMesh);

  RenderStateInfo oInfo = {};
  oInfo.m_sVSFilename = "Assets/Shaders/Vertex/EnvMapVertex.vs";
  oInfo.m_sGSFilename = "Assets/Shaders/Geometry/EnvMapGeometry.gs";
  oInfo.m_sPSFilename = "Assets/Shaders/Pixel/EnvMapPixel.ps";
  oInfo.m_bBlendEnabled = false;
  oInfo.m_bDepthRead = false;
  oInfo.m_bDepthWrite = false;
  oInfo.m_eCullMode = CullMode::BACK;  

  Pass oEnvMapPass = {};
  oEnvMapPass.Configure(oInfo, m_pEnvMapSpec.get(), 0u);
  oEnvMapPass.Setup();

  MaterialInstance oMatInstance = {};
  oMatInstance.AddTexture(std::move(pHDRI));
  oMatInstance.Configure();
  oMatInstance.SetupSubState(nullptr);

  /*
  
  data.viewProj[0] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationY(_PI2)), proj));
  data.viewProj[1] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationY(-_PI2)), proj));
  data.viewProj[2] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(-_PI2)), proj));
  data.viewProj[3] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationX(_PI2)), proj));
  data.viewProj[4] = DirectX::XMMatrixTranspose(proj);
  data.viewProj[5] = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationY(_PI)), proj));
  */

  constexpr float _PI2 = glm::half_pi<float>(); // 90 degrees
  constexpr float _PI = glm::pi<float>();

  glm::mat4 proj = glm::perspectiveLH(_PI2, 1.0f, 0.1f, 10.0f);
  
  glm::mat4 aViewProjs[6];
  aViewProjs[0] = proj * glm::inverse(glm::rotate(glm::mat4(1.0f),  _PI2, glm::vec3(0, 1, 0)));
  aViewProjs[1] = proj * glm::inverse(glm::rotate(glm::mat4(1.0f), -_PI2, glm::vec3(0, 1, 0)));
  aViewProjs[2] = proj * glm::inverse(glm::rotate(glm::mat4(1.0f), -_PI2, glm::vec3(1, 0, 0)));
  aViewProjs[3] = proj * glm::inverse(glm::rotate(glm::mat4(1.0f),  _PI2, glm::vec3(1, 0, 0)));
  aViewProjs[4] = proj;
  aViewProjs[5] = proj * glm::inverse(glm::rotate(glm::mat4(1.0f),  _PI,  glm::vec3(0, 1, 0)));

  oEnvMapPass.SetMat4("aViewProjs", &aViewProjs[0][0][0]);

  Engine::GetInstance()->GetWindow()->BeginDrawOffline();
  
  GeometryRenderStep oEnvMapStep("", std::vector<Texture2D*>(), m_pEnvMapSpec.get(), false, false);

  Job oJob = {};
  oJob.m_pMaterial = &oMatInstance;
  oJob.m_pMesh = &oCubeMesh;
  oJob.m_pMeshTransform = &Transform::GetIdentity();
  oJob.m_pPass = &oEnvMapPass;  

  oEnvMapStep.SubmitJob(std::move(oJob));

  oEnvMapStep.Setup();

  oEnvMapStep.Execute(&oCamera, &Transform::GetIdentity());

  m_pEnvMapSpec->Unbind();

  Engine::GetInstance()->GetWindow()->EndDraw();

  m_pLightCBuff->GetData()->m_uNumLights = 0u;
}

void Renderer::ShutDownPreWindow()
{
  glfwTerminate();

  m_lstCamViews.clear();

  m_lstRenderPipelines.clear();

  m_pLightCBuff.reset();

  m_pEnvMapDiff.reset();

  m_pEnvMapSpec.reset();
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
    RenderStep* pRenderStep = pPipeline ? pPipeline->GetRenderStep(pPass->GetRenderStepId()) : nullptr;

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
  m_pLightCBuff->SetupRenderSubState("LightBuffer", STAGE_PIXEL, _eFrequency);
}

void Renderer::SetupSubStateShadowMaps(ResourceFrequency _eFrequency)
{
  static const char aNames[4][11] = { "ShadowMap0", "ShadowMap1", "ShadowMap2", "ShadowMap3" };
  for (unsigned int i = 0u; i < m_lstShadowViews.size(); i++)
  {
    const RenderTarget* pShadowMap = m_lstShadowViews[i].m_pShadowMap;
    pShadowMap->GetDepthStencilTexture()->SetupRenderSubState(aNames[i], STAGE_PIXEL, _eFrequency);
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

      owner_ptr<GeometryRenderStep> pShadowStep = Factory::Create<GeometryRenderStep>("", std::vector<Texture2D*>(), rShadowView.m_pShadowMap, false, false);

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

    //m_pEnvMapDiff->Bind();
    m_pEnvMapSpec->GetColorTextures()[0]->Bind();

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
    Engine::GetInstance()->GetWindow()->Present();
  }
  

  m_lstCamViews.clear();
  m_lstShadowViews.clear();
  m_lstShadowJobs.clear();
  m_pLightCBuff->GetData()->m_uNumLights = 0u;
  m_pLightCBuff->GetData()->m_uNumShadows = 0u;
}
