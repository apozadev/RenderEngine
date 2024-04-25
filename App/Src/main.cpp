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

// TODO: Descriptor layout, pools y sets. Lo movemos del CBuffer al RenderState. Implementar DescriptorUtils. 
// Los poolSize de descriptors de CBuffer se multiplican por numero de swpchain images. Las imagenes NO hace falta.
// Primero crearemos lo que podamos de RenderState. Hacemos un visitor por los recursos del material que metan sus cosas en los Builders. Al final se hace un Build con la info.


int main(){

  try
  {

  Engine::GetInstance()->Initialize();
  Window* pWindow = Engine::GetInstance()->CreateNewWindow(640, 420, "App");  

  Scene* pScene = Engine::GetInstance()->CreateScene(pWindow);

  Entity* pEntity = pScene->AddEntity();

  std::vector<Vertex> lstVertices {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
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
  pModelComp->GetMaterial(0).Setup();

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