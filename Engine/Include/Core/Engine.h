#pragma once

#include "Singleton.h"

namespace core {

  class Window;

  class Engine : public Singleton<Engine>
  {
  public:
    int Initialize();
    int Run();
    int ScheduleShutDown();

  private:

    bool ShouldShutDown();
    void ShutDown();

    bool m_bRunning;

    float m_fTargetFPS;
    float m_fGameTime;
    float m_fDt;
  };
}