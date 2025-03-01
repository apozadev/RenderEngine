#include <iostream>
#include <vector>

#include <fstream>

#include <Windows.h>

#include "SceneEditor.h"

#include "Core/Engine.h"
#include "Core/Exception.h"
#include "Core/SceneLoading.h"
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

#include "Reflection/ReflectionHelper.h"

#include <string>

// TODO:
// Shader linkage error

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

    RenderPipelineConfig oConfig = {};
    reflection::ReflectionHelper::Deserialize("Assets/RenderInfos/RenderInfo.xml", &oConfig);

    Renderer::GetInstance()->AddRenderPipeline(std::move(oConfig));

    Scene* pScene = Engine::GetInstance()->CreateScene();

    LoadScene("Assets/Scenes/TestScene.xml", *pScene);

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