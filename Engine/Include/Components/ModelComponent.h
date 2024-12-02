#pragma once

#include "Graphics/Mesh.h"

#include "Graphics/MaterialInstance.h"

#include <vector>
#include "Core/Component.h"
#include "Graphics/Mesh.h"

class ModelComponent : public Component
{

public:

  struct MeshMaterialPair
  {
    owner_ptr<Mesh> m_pMesh;
    unsigned int m_uMatIdx;
  };

public:
  ModelComponent(const char* _sFilename, Material* _pMaterial);  
  ~ModelComponent();

  void AddMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx);

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