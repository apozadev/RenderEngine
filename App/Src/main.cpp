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
  Window* pWindow = Engine::GetInstance()->CreateNewWindow(1080, 920, "App");  

  Scene* pScene = Engine::GetInstance()->CreateScene(pWindow);

  Entity* pCameraEntity = pScene->AddEntity();

  pCameraEntity->AddComponent<CameraComponent>(pWindow);

  Entity* pModelEntity1 = pScene->AddEntity();
  Entity* pModelEntity2 = pScene->AddEntity();

  pModelEntity1->AddComponent<ModelComponent>("Assets/Models/cyborg/cyborg.obj", pWindow);    
  pModelEntity2->AddComponent<ModelComponent>("Assets/Models/demon.fbx", pWindow);    

  pModelEntity1->GetMutableLocalTransform().SetPos({ 0.f,0.f, -10.f });
  pModelEntity2->GetMutableLocalTransform().SetPos({ 5.f,0.f, -10.f });
  pModelEntity2->GetMutableLocalTransform().SetScale({ 0.015f, 0.015f, 0.015f});  

  //Entity* pModelEntity1 = pScene->AddEntity();
  //pModelEntity1->AddComponent<ModelComponent>("Assets/Models/teapot.obj", pWindow);
  //pModelEntity1->GetMutableLocalTransform().SetPos({ 0.f,0.f, -10.f });º

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