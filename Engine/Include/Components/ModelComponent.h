#include "Graphics/Mesh.h"

#include "Graphics/MaterialInstance.h"

#include <vector>
#include "Core/Component.h"


class ModelComponent : public Component
{

  struct MeshMaterialPair
  {
    Mesh m_oMesh;
    unsigned int m_uMatIdx;
  };

public:
  //ModelComponent() {};
  //~ModelComponent() {};

  void AddMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, unsigned int _uMaterialIdx, Window* _pWindow);

  void AddMaterial(MaterialInstance&& _rMaterial)
  {
    m_lstMaterials.push_back(std::move(_rMaterial));
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
  virtual void Update(float _fTimeStep) override;

private:
  std::vector<MeshMaterialPair> m_lstMeshes;

  std::vector<MaterialInstance> m_lstMaterials;
};