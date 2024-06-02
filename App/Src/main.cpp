#include <iostream>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Exception.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Texture2D.h"
#include "Components/ModelComponent.h"

#include <string>

int main(){

  try
  {

  Engine::GetInstance()->Initialize();
  Window* pWindow = Engine::GetInstance()->CreateNewWindow(640, 420, "App");  

  Scene* pScene = Engine::GetInstance()->CreateScene(pWindow);

  Entity* pEntity = pScene->AddEntity();

  /*std::vector<Vertex> lstVertices {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
  };

  std::vector<uint16_t> lstIndices{
    0,1,2,
    2,3,0
  };

  Material oMaterial(pWindow);  
  
  ModelComponent* pModelComp = pEntity->AddComponent<ModelComponent>();  
  pModelComp->AddMaterial(MaterialInstance(&oMaterial));
  pModelComp->AddMesh(lstVertices, lstIndices, 0u, pWindow);

  pModelComp->GetMaterial(0).AddResource<Texture2D>(std::string("Assets/Images/ripple.png"));
  pModelComp->GetMaterial(0).Setup();*/

  pEntity->AddComponent<ModelComponent>("Assets/Models/teapot.obj", pWindow);

  pEntity->GetLocalTransform().SetPos({ 0.f,0.f, -10.f });

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