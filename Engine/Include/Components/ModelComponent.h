#pragma once

#include "Graphics/Mesh.h"

#include "Graphics/MaterialInstance.h"

#include <vector>
#include "Core/Component.h"


class ModelComponent : public Component
{

public:

  struct MeshMaterialPair
  {
    Mesh m_oMesh;
    unsigned int m_uMatIdx;
  };

public:
  ModelComponent(const char* _sFilename, Window* _pWindow);
  //~ModelComponent() {};

  void AddMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx, Window* _pWindow);

  MaterialInstance& AddMaterialInstance(Material* _pMaterial)
  {
    m_lstMaterials.emplace_back(_pMaterial);
    return m_lstMaterials[m_lstMaterials.size() - 1];
  }

  MaterialInstance& GetMaterial(unsigned int _uIdx)
  {
    return m_lstMaterials[_uIdx];
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

  std::vector<MaterialInstance> m_lstMaterials;  
};