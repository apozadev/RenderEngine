#pragma once

#include "Graphics/Mesh.h"

#include "Graphics/MaterialInstance.h"

#include <vector>
#include <string>
#include "Core/Component.h"
#include "Core/ComponentDesc.h"
#include "Graphics/Mesh.h"

#include "Reflection/ReflectionMacros.h"

class ModelComponent : public Component
{

public:

  REFLECT()

  struct MeshMaterialPair: public BaseObject
  {
    REFLECT_BASE()

    MeshMaterialPair() = default;
    MeshMaterialPair(owner_ptr<Mesh>&& _pMesh, unsigned int _uMatIdx) : m_pMesh(std::move(_pMesh)), m_uMatIdx(_uMatIdx) {}

    owner_ptr<Mesh> m_pMesh;
    unsigned int m_uMatIdx;
  };

public:
  ModelComponent() = default;
  ModelComponent(const char* _sFilename, Material* _pMaterial);  
  ~ModelComponent();

  void Configure() override;

  void AddMesh(const std::vector<Vertex>& _lstVertices, const std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx);

  void AddMaterialInstance(owner_ptr<MaterialInstance>&& _pMaterial)
  {
    m_lstMaterials.emplace_back(std::move(_pMaterial));    
  }

  MaterialInstance* GetMaterial(unsigned int _uIdx)
  {
    return m_lstMaterials[_uIdx].get();
  }

  unsigned int GetMaterialCount() const
  {
    return static_cast<unsigned int>(m_lstMaterials.size());
  }

protected:

  virtual void Start() override;
  virtual void PreTransformUpdate(float _fTimeStep) override;
  virtual void Update(float _fTimeStep) override;

private:
  std::vector<MeshMaterialPair> m_lstMeshes;

  std::vector<owner_ptr<MaterialInstance>> m_lstMaterials;  
};

DECLARE_COMPONENT_DESC_BEGIN(ModelComponentDesc, ModelComponent)
  std::string m_sModelFilename;
  std::string m_sMaterialFilename;
DECLARE_COMPONENT_DESC_END(ModelComponentDesc)

DECLARE_REFLECTION_VECTOR(ModelComponent::MeshMaterialPair)
