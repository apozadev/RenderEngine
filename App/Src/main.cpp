#include <iostream>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Exception.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Components/ModelComponent.h"

int main(){

  try
  {

  core::Engine::GetInstance()->Initialize();
  Window* pWindow = Renderer::GetInstance()->CreateNewWindow(640, 420, "App");  

  Scene* pScene = core::Engine::GetInstance()->CreateScene(pWindow);

  Entity* pEntity = pScene->AddEntity();

  std::vector<Vertex> lstVertices {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
  };

  std::vector<uint16_t> lstIndices{
    0,1,2,
    2,3,0
  };
  
  ModelComponent* pModelComp = pEntity->AddComponent<ModelComponent>();
  pModelComp->AddMesh(Mesh(lstVertices, lstIndices, pWindow)); 

  core::Engine::GetInstance()->Run();

  }
  catch (const core::Exception& e)
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