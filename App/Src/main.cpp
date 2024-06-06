#include <iostream>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Exception.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Texture2D.h"
#include "Components/ModelComponent.h"
#include "Components/CameraComponent.h"

#include <string>

int main(){

  try
  {

  Engine::GetInstance()->Initialize();
  Window* pWindow = Engine::GetInstance()->CreateNewWindow(640, 420, "App");  

  Scene* pScene = Engine::GetInstance()->CreateScene(pWindow);

  Entity* pCameraEntity = pScene->AddEntity();

  pCameraEntity->AddComponent<CameraComponent>(pWindow);

  Entity* pModelEntity = pScene->AddEntity();

  pModelEntity->AddComponent<ModelComponent>("Assets/Models/teapot.obj", pWindow);

  pModelEntity->GetMutableLocalTransform().SetPos({ 0.f,0.f, -10.f });

  pScene->Build();

  Engine::GetInstance()->Run();

  }
  catch (const Exception& e)
  {
    MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
  }
  catch (const std::exception& e)
  {
    MessageBoxA(nullptr, e.what(), "Generic Exception", MB_OK | MB_ICONEXCLAMATION);
  }
  catch (...)
  {
    MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
  }

  return -1;

}