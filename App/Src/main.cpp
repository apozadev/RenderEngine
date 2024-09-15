#include <iostream>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Exception.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Texture2D.h"
#include "Graphics/BlendEnums.h"
#include "Components/ModelComponent.h"
#include "Components/CameraComponent.h"
#include "Graphics/MaterialLibrary.h"
#include "Graphics/RenderPipelineConfig.h"

#include <string>

int main(){

  try
  {

  Engine::GetInstance()->Initialize();
  Window* pWindow = Engine::GetInstance()->CreateNewWindow(1080, 920, "App");  

  Scene* pScene = Engine::GetInstance()->CreateScene(pWindow);

  RenderStepConfig oStep = {};
  oStep.m_sRenderTargetId = "DEFAULT";

  RenderPipelineConfig oConfig = {};
  oConfig.m_sId = "TEST";
  oConfig.m_lstSteps.push_back(std::move(oStep));

  Renderer::GetInstance()->AddRenderPipeline(pWindow, std::move(oConfig));

  Entity* pCameraEntity = pScene->AddEntity();

  pCameraEntity->AddComponent<CameraComponent>(pWindow);

  Entity* pModelEntity1 = pScene->AddEntity();
  Entity* pModelEntity2 = pScene->AddEntity();

  Material* pMaterial = MaterialLibrary::GetInstance()->CreateMaterial(pWindow
    , "Assets/Shaders/Vertex/VertexShader.hlsl"
    , "Assets/Shaders/Pixel/PixelShader"
    , true
    , BlendOp::BLEND_OP_ADD
    , BlendFactor::BLEND_SRC_ALPHA
    , BlendFactor::BLEND_INV_SRC_ALPHA
    , false
    , true);

  pMaterial->Setup();

  pModelEntity1->AddComponent<ModelComponent>("Assets/Models/cyborg/cyborg.obj", pMaterial, pWindow);    
  pModelEntity2->AddComponent<ModelComponent>("Assets/Models/demon.fbx", pMaterial, pWindow);    

  pModelEntity1->GetMutableLocalTransform().SetPos({ 0.f,0.f, -10.f });
  pModelEntity2->GetMutableLocalTransform().SetPos({ 5.f,0.f, -10.f });
  pModelEntity2->GetMutableLocalTransform().SetScale({ 0.015f, 0.015f, 0.015f});    

  pScene->Build();

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