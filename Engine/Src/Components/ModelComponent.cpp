#include "Components/ModelComponent.h"
#include "Graphics/Renderer.h"
#include "Core/Entity.h"

void ModelComponent::AddMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx, Window* _pWindow)
{
  m_lstMeshes.push_back({
    std::move(Mesh(_lstVertices, _lstIndices, &m_lstMaterials[_uMaterialIdx], _pWindow))
    , _uMaterialIdx
    });
}

void ModelComponent::Start()
{

}

void ModelComponent::Update(float _fTimeStep)
{
  for (MeshMaterialPair& rMeshMat : m_lstMeshes)
  {    
    Renderer::GetInstance()->SubmitMesh(&rMeshMat.m_oMesh, &m_lstMaterials[rMeshMat.m_uMatIdx], &m_pEntity->GetGlobalTransform());
  }
}
