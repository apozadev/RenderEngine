#include "Graphics/Mesh.h"

#include <vector>
#include "Core/Component.h"


class ModelComponent : public Component
{
public:
  //ModelComponent() {};
  //~ModelComponent() {};

  void AddMesh(Mesh&& _rMesh)
  {
    m_lstMeshes.push_back(std::move(_rMesh));
  }

protected:

  virtual void Start() override;
  virtual void Update(float _fTimeStep) override;

private:
  std::vector<Mesh> m_lstMeshes;
};