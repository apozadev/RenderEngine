#pragma once

#include <vector>

#include "Core/Singleton.h"

#include "Graphics/Vertex.h"

class Window;
class Mesh;
class MaterialInstance;
class Transform;

class Renderer : public Singleton<Renderer>
{

  struct Job
  {
    Mesh* m_pMesh;
    const MaterialInstance* m_pMaterial;
    const Window* m_pWindow;
    const Transform* m_pTransform;
    uint64_t m_uKey;
  };

public:  

  void Initialize();
  void ShutDown();  

  //Mesh* CreateMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, Window* _pWindow);

  void SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform);
  
  void Draw();

private:

  static bool Renderer::compareJob(const Job& j1, const Job& j2);

  std::vector<Job> m_lstJobs;  
};