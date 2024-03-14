#pragma once

#include <vector>

#include "Core/Singleton.h"

#include "Graphics/Vertex.h"

class Window;
class Mesh;

class Renderer : public core::Singleton<Renderer>
{

  struct Job
  {
    Mesh* m_pMesh;
    Window* m_pWindow;
    uint64_t m_uKey;
  };

public:  

  void Initialize();
  void ShutDown();

  Window* CreateNewWindow(int _iWidth, int _iHeight, const char* _sTitle);  
  const std::vector<Window*>& GetWindows() { return m_lstWindows; }
  void SetUsingWindow(Window* _pWindow);

  Mesh* CreateMesh(std::vector<Vertex>& _lstVertices, Window* _pWindow);

  void SubmitMesh(Mesh* _pMesh);

  void UpdateWindows();
  void Draw();

private:

  static bool Renderer::compareJob(const Job& j1, const Job& j2);

  std::vector<Job> m_lstJobs;
  std::vector<Window*> m_lstWindows;
};