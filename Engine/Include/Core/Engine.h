#pragma once

#include "Singleton.h"

namespace core {

  class Window;

  class Engine : public Singleton<Engine>
  {
  public:
    int Init();    
    void Run();
    int ShutDown();

  private:

    bool ShouldShutDown();

    bool m_bRunning;

    float m_fTargetFPS;
    float m_fGameTime;
    float m_fDt;
  };
}