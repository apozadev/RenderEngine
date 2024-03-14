#pragma once

#include <vector>

#include "Singleton.h"

class Window;
class Mesh;

namespace core {  

  class Engine : public Singleton<Engine>
  {
  public:
    int Initialize();
    int Run();
    int ScheduleShutDown();

    void AddMesh(Mesh* _pMesh);

  private:

    bool ShouldShutDown();
    void ShutDown();

    bool m_bRunning;

    float m_fTargetFPS;
    float m_fGameTime;
    float m_fDt;

    std::vector<Mesh*> m_lstMeshes;
  };
}