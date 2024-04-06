#include "Components/ModelComponent.h"
#include "Graphics/Renderer.h"
#include "Core/Entity.h"

void ModelComponent::Start()
{

}

void ModelComponent::Update(float _fTimeStep)
{
  for (Mesh& rMesh : m_lstMeshes)
  {    
    Renderer::GetInstance()->SubmitMesh(&rMesh, &m_pEntity->GetGlobalTransform());
  }
}