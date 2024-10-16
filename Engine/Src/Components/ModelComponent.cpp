#include "Components/ModelComponent.h"
#include "Graphics/Renderer.h"
#include "Core/Entity.h"
#include "Util/ModelLoader.h"

ModelComponent::ModelComponent(const char* _sFilename, Material* _pMaterial, Window* _pWindow)
{
  ModelLoader::GetInstance()->LoadModel(_sFilename, _pMaterial, _pWindow, this);
}

void ModelComponent::AddMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx, Window* _pWindow)
{
  m_lstMeshes.push_back({
    std::move(Mesh(_lstVertices, _lstIndices, _pWindow))
    , _uMaterialIdx
    });
}

void ModelComponent::Start()
{  
}

void ModelComponent::PreTransformUpdate(float _fTimeStep) 
{
  //constexpr float fSpeed = 1.f;
  //Transform& oTr = m_pEntity->GetMutableLocalTransform();
  //oTr.SetRot(glm::quat(glm::vec3(0.f, fSpeed * _fTimeStep, 0.f)) * oTr.GetRot());
}

void ModelComponent::Update(float _fTimeStep)
{
  for (MeshMaterialPair& rMeshMat : m_lstMeshes)
  {    
    Renderer::GetInstance()->SubmitMesh(&rMeshMat.m_oMesh, &m_lstMaterials[rMeshMat.m_uMatIdx], &m_pEntity->GetGlobalTransform());
  }
}
