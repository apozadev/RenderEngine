#include <iostream>
#include <vector>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Exception.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Pass.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Texture2D.h"
#include "Graphics/BlendEnums.h"
#include "Components/ModelComponent.h"
#include "Components/CameraComponent.h"
#include "Graphics/MaterialLibrary.h"
#include "Graphics/RenderPipelineConfig.h"

#include <string>

/*
* Lista de jobs por renderstep, vamos metiendolos donde toca en el submit
* Cada renderstep ordena con transparent o opaque
*/

int main(){

  try
  {

  // Startup
  Engine::GetInstance()->Initialize();
  Window* pWindow = Engine::GetInstance()->CreateNewWindow(1080, 920, "App");  

  Scene* pScene = Engine::GetInstance()->CreateScene(pWindow);

  // Render info config
  {
    RenderTargetConfig oTarget = {};
    oTarget.m_sId = "GBUFFER";
    oTarget.m_bHasDepthStencil = true;
    oTarget.m_eFormat = ImageFormat::R8G8B8A8;
    oTarget.m_iWidth = pWindow->GetWidth();
    oTarget.m_iHeight = pWindow->GetHeight();
    oTarget.m_uMipLevels = 1u;
    oTarget.m_uMsaaSamples = 1u;
    oTarget.m_uNumColorTextures = 1u;

    RenderStepInputConfig oInput = {};
    oInput.m_iImageIdx = 0;
    oInput.m_sTargetId = "GBUFFER";

    RenderStepConfig oStep1 = {};
    oStep1.m_sRenderTargetId = "GBUFFER";

    RenderStepConfig oStep2 = {};    
    oStep2.m_lstInputs.push_back(std::move(oInput));
    oStep2.m_sRenderTargetId = "DEFAULT";    

    RenderPipelineConfig oConfig = {};
    oConfig.m_sId = "TEST";
    oConfig.m_lstSteps.push_back(std::move(oStep1));
    oConfig.m_lstSteps.push_back(std::move(oStep2));
    oConfig.m_lstRenderTargets.push_back(std::move(oTarget));

    Renderer::GetInstance()->AddRenderPipeline(pWindow, std::move(oConfig));
  }

  // Create entities

  Entity* pCameraEntity = pScene->AddEntity();

  pCameraEntity->AddComponent<CameraComponent>(pWindow);

  Entity* pModelEntity1 = pScene->AddEntity();
  Entity* pModelEntity2 = pScene->AddEntity();

  std::vector<Pass> lstPasses;

  Pass oPass0(pWindow
    , "Assets/Shaders/Vertex/VertexShader.hlsl"
    , "Assets/Shaders/Pixel/GBuffPixel.hlsl"
    , false
    , BlendOp::BLEND_OP_ADD
    , BlendFactor::BLEND_SRC_ALPHA
    , BlendFactor::BLEND_INV_SRC_ALPHA
    , true
    , true
    , "TEST"
    , 0);

  Pass oPass1(pWindow
    , "Assets/Shaders/Vertex/VertexShader.hlsl"
    , "Assets/Shaders/Pixel/PixelShader.hlsl"
    , false
    , BlendOp::BLEND_OP_ADD
    , BlendFactor::BLEND_SRC_ALPHA
    , BlendFactor::BLEND_INV_SRC_ALPHA
    , true
    , true
    , "TEST"
    , 1);

  lstPasses.push_back(std::move(oPass0));
  lstPasses.push_back(std::move(oPass1));

  Material* pMaterial = MaterialLibrary::GetInstance()->CreateMaterial(pWindow, std::move(lstPasses));

  pMaterial->Setup();

  pModelEntity1->AddComponent<ModelComponent>("Assets/Models/cyborg/cyborg.obj", pMaterial, pWindow);    
  pModelEntity2->AddComponent<ModelComponent>("Assets/Models/demon.fbx", pMaterial, pWindow);    

  pModelEntity1->GetMutableLocalTransform().SetPos({ 0.f,0.f, -10.f });
  pModelEntity2->GetMutableLocalTransform().SetPos({ 5.f,0.f, -10.f });
  pModelEntity2->GetMutableLocalTransform().SetScale({ 0.015f, 0.015f, 0.015f});    

  pScene->Build();

  // Go!
  Engine::GetInstance()->Run();

  }
  catch (const Exception& e)
  {
    MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
  }
  /*
  catch (const std::exception& e)
  {
    MessageBoxA(nullptr, e.what(), "Generic Exception", MB_OK | MB_ICONEXCLAMATION);
  }
  catch (...)
  {
    MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
  }*/

  return -1;

}