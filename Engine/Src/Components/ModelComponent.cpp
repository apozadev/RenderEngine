#include "Components/ModelComponent.h"
#include "Graphics/Renderer.h"
#include "Graphics/MaterialLibrary.h"
#include "Core/Entity.h"
#include "Util/ModelLoader.h"

#include "Memory/Factory.h"

#include "Reflection/ReflectionImplMacros.h"

ModelComponent::ModelComponent(const char* _sFilename, Material* _pMaterial)
{
  
}

ModelComponent::~ModelComponent()
{
}

void ModelComponent::Configure()
{
  const ModelComponentDesc* pModelCompDesc = dynamic_cast<const ModelComponentDesc*>(m_pComponentDesc.get());

  const Material* pMaterial = MaterialLibrary::GetInstance()->LoadMaterial(pModelCompDesc->m_sMaterialFilename.c_str());

  static const std::string s_sQuad("quad");
  static const std::string s_sCube("cube");

  if (s_sQuad == pModelCompDesc->m_sModelFilename)
  {
    SetupQuadModel(pMaterial, this);
  }
  else if (s_sCube == pModelCompDesc->m_sModelFilename)
  {
    SetupCubeModel(pMaterial, this);
  }
  else
  {
    LoadModel(pModelCompDesc->m_sModelFilename.c_str(), pMaterial, this);
  }  
}

void ModelComponent::AddMesh(const std::vector<Vertex>& _lstVertices, const std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx)
{

  owner_ptr<Mesh> pMesh = Factory::Create<Mesh>();
  pMesh->Initialize(_lstVertices, _lstIndices);
  m_lstMeshes.push_back(MeshMaterialPair( std::move(pMesh), _uMaterialIdx ));
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
    Renderer::GetInstance()->SubmitMesh(rMeshMat.m_pMesh.get(), m_lstMaterials[rMeshMat.m_uMatIdx].get(), &m_pEntity->GetGlobalTransform(), true);
  }
}

typedef ModelComponent::MeshMaterialPair Model_MeshMaterialPair;
REFLECT_STRUCT_BASE_BEGIN(ModelComponent::MeshMaterialPair)
REFLECT_STRUCT_MEMBER(m_pMesh)
REFLECT_STRUCT_MEMBER(m_uMatIdx)
REFLECT_STRUCT_END(Model_MeshMaterialPair)

IMPLEMENT_REFLECTION_VECTOR(ModelComponent::MeshMaterialPair)

REFLECT_STRUCT_BEGIN(ModelComponent, Component)
REFLECT_STRUCT_MEMBER(m_lstMeshes)
REFLECT_STRUCT_MEMBER(m_lstMaterials)
REFLECT_STRUCT_END(ModelComponent)

IMPLEMENT_COMPONENT_DESC_BEGIN(ModelComponentDesc)
REFLECT_STRUCT_MEMBER(m_sModelFilename)
REFLECT_STRUCT_MEMBER(m_sMaterialFilename)
IMPLEMENT_COMPONENT_DESC_END(ModelComponentDesc)


