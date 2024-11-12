#include "Components/ModelComponent.h"
#include "Graphics/Renderer.h"
#include "Core/Entity.h"
#include "Util/ModelLoader.h"

ModelComponent::ModelComponent(const char* _sFilename, Material* _pMaterial)
{
  static const std::string s_sQuad("quad");

  if (s_sQuad == _sFilename)
  {
    ModelLoader::GetInstance()->SetupQuadModel(_pMaterial, this);
  }
  else
  {
    ModelLoader::GetInstance()->LoadModel(_sFilename, _pMaterial, this);
  }
}

ModelComponent::~ModelComponent()
{
}

void ModelComponent::AddMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx)
{

  pooled_ptr<Mesh> pMesh = Mesh::CreateInstance();
  pMesh->Initialize(_lstVertices, _lstIndices);
  m_lstMeshes.push_back(MeshMaterialPair{ std::move(pMesh), _uMaterialIdx });
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
    Renderer::GetInstance()->SubmitMesh(rMeshMat.m_pMesh.get(), &m_lstMaterials[rMeshMat.m_uMatIdx], &m_pEntity->GetGlobalTransform());
  }
}
