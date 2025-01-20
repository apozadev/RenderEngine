#include <iostream>
#include <vector>

#include <Windows.h>

#include "SceneEditor.h"

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
#include "Components/DirLightComponent.h"
#include "Graphics/MaterialLibrary.h"
#include "Graphics/RenderPipelineConfig.h"
#include "Memory/Factory.h"

#include <string>

// TODO:
// Hacer componentes owner_ptr tambien. 
// Camara de sombras no ve mallas. ajustar posición de luces.

void PostUpdateCallback(float _fDt)
{
  for (Scene& rScene : Engine::GetInstance()->GetScenes())
  {
    ViewScene(&rScene);
  }
}

int main()
{

  try
  {

    // Startup
    Engine::GetInstance()->Initialize(1080, 920, "App");

    Scene* pScene = Engine::GetInstance()->CreateScene();

    // Render info config
    {
      RenderTargetConfig oTarget = {};
      oTarget.m_sId = "GBUFFER";
      oTarget.m_bHasDepthStencil = true;
      oTarget.m_eFormat = ImageFormat::R8G8B8A8;
      oTarget.m_iWidth = -1;
      oTarget.m_iHeight = -1;
      oTarget.m_uMipLevels = 1u;
      oTarget.m_uMsaaSamples = 1u;
      oTarget.m_uNumColorTextures = 1u;

      RenderStepInputConfig oInput = {};
      oInput.m_iImageIdx = 0;
      oInput.m_sTargetId = "GBUFFER";

      RenderStepConfig oStep1 = {};
      oStep1.m_sRenderTargetId = "GBUFFER";
      oStep1.m_bOrderTranslucent = false;

      RenderStepConfig oStep2 = {};
      oStep2.m_lstInputs.push_back(std::move(oInput));
      oStep2.m_sRenderTargetId = "DEFAULT";
      oStep2.m_bOrderTranslucent = false;

      RenderStepConfig oStep3 = {};
      oStep3.m_sRenderTargetId = "DEFAULT";
      oStep3.m_bOrderTranslucent = true;

      RenderPipelineConfig oConfig = {};
      oConfig.m_sId = "TEST";
      oConfig.m_lstSteps.push_back(std::move(oStep1));
      oConfig.m_lstSteps.push_back(std::move(oStep2));
      oConfig.m_lstSteps.push_back(std::move(oStep3));
      oConfig.m_lstRenderTargets.push_back(std::move(oTarget));

      Renderer::GetInstance()->AddRenderPipeline(std::move(oConfig));
    }

    Factory::PushGlobalMode(false);

    // Create entities

    Entity* pCameraEntity = pScene->AddEntity();

    pCameraEntity->AddComponent<CameraComponent>();
    pCameraEntity->GetMutableLocalTransform().SetPos({ 0.f, 2.f, 0.f });

    /*pCameraEntity->GetMutableLocalTransform().SetPos({ 0.f, 2.f, -20.f });
    pCameraEntity->GetMutableLocalTransform().SetRot(glm::angleAxis(3.14159f, glm::vec3{ 0.f, 1.f, 0.f }));*/

    Entity* pModelEntity1 = pScene->AddEntity();
    Entity* pModelEntity2 = pScene->AddEntity();
    Entity* pGridEntity = pScene->AddEntity();
    //Entity* pLightEntity = pScene->AddEntity();
    Entity* pLightEntity2 = pScene->AddEntity();

    Material* pMaterial = MaterialLibrary::GetInstance()->CreateMaterial();
    {
      owner_ptr<Pass> pPass0 = Factory::Create<Pass>();
      pPass0->Configure(
        "Assets/Shaders/Vertex/VertexShader.hlsl"
        , "Assets/Shaders/Pixel/GBuffPixel.hlsl"
        , false
        , BlendOp::BLEND_OP_ADD
        , BlendFactor::BLEND_SRC_ALPHA
        , BlendFactor::BLEND_INV_SRC_ALPHA
        , true
        , true
        , "TEST"
        , 0
        , 0u);
      pMaterial->AddPass(std::move(pPass0));

      owner_ptr<Pass> pPass1 = Factory::Create<Pass>();
      pPass1->Configure(
        "Assets/Shaders/Vertex/VertexShader.hlsl"
        , "Assets/Shaders/Pixel/PixelShader.hlsl"
        , false
        , BlendOp::BLEND_OP_ADD
        , BlendFactor::BLEND_SRC_ALPHA
        , BlendFactor::BLEND_INV_SRC_ALPHA
        , true
        , true
        , "TEST"
        , 1
        , 0u);
      pMaterial->AddPass(std::move(pPass1));
    }
    pMaterial->Setup();

    float aTint[4] = { 1.f, 1.f, 1.f, 1.f };
    pMaterial->GetPasses()[1]->SetVec4("vTint", aTint);
    pMaterial->GetPasses()[1]->SetFloat("fMult", 1.f);

    Material* pGridMaterial = MaterialLibrary::GetInstance()->CreateMaterial();
    {
      owner_ptr<Pass> pPass0 = Factory::Create<Pass>();
      pPass0->Configure(
        "Assets/Shaders/Vertex/VertexShader.hlsl"
        , "Assets/Shaders/Pixel/GridPixel.hlsl"
        , true
        , BlendOp::BLEND_OP_ADD
        , BlendFactor::BLEND_SRC_ALPHA
        , BlendFactor::BLEND_INV_SRC_ALPHA
        , false
        , true
        , "TEST"
        , 2
        , 0u);

      pGridMaterial->AddPass(std::move(pPass0));
    }
    pGridMaterial->Setup();

    pModelEntity1->AddComponent<ModelComponent>("Assets/Models/cyborg/cyborg.obj", pMaterial);
    pModelEntity2->AddComponent<ModelComponent>("Assets/Models/demon.fbx", pMaterial);
    pGridEntity->AddComponent<ModelComponent>("quad", pGridMaterial);

    pModelEntity1->GetMutableLocalTransform().SetPos({ 0.f,0.f, -10.f });

    pModelEntity2->GetMutableLocalTransform().SetPos({ 2.f,0.f, -10.f });
    pModelEntity2->GetMutableLocalTransform().SetScale({ 0.015f, 0.015f, 0.015f });

    pGridEntity->GetMutableLocalTransform().SetRot(glm::angleAxis(3.14159f / 2.f, glm::vec3{ 1.f, 0.f, 0.f }));
    pGridEntity->GetMutableLocalTransform().SetPos({ 0.f, 0.f, -10.f });
    pGridEntity->GetMutableLocalTransform().SetScale({ 99.f, 99.f, 99.f });

    //pLightEntity->AddComponent<DirLightComponent>();
    pLightEntity2->AddComponent<DirLightComponent>()->SetColor({ 5.5f, 0.5f, 0.3f });
    pLightEntity2->GetMutableLocalTransform().Translate({ 0.f, 0.f, -20.f });
    //pLightEntity2->GetMutableLocalTransform().SetRot(glm::angleAxis(3.14159f, glm::vec3{ 0.f, 1.f, 0.f }));

    pScene->Build();

    // Imgui

    Engine::GetInstance()->SetPostUpdateCallback(PostUpdateCallback);

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