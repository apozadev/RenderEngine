#pragma once

#include <vector>

#include "Singleton.h"

#include "Core/Scene.h"

class Mesh;
class Window;

namespace core {    

  class Engine : public Singleton<Engine>
  {
    struct SceneWindowPair
    {
      Scene m_oScene;
      Window* m_pWindow;
    };

  public:
    int Initialize();
    int Run();
    int ScheduleShutDown();

    void AddMesh(Mesh* _pMesh);

    Scene* CreateScene(Window* _pWindow);

    bool IsRunning() { return m_bRunning; }

  private:

    bool ShouldShutDown();
    void ShutDown();

    bool m_bRunning;

    float m_fTargetFPS;
    float m_fGameTime;
    float m_fDt;

    std::vector<Mesh*> m_lstMeshes;

    std::vector<SceneWindowPair> m_lstScenes;
  };
}