#include <thread>

#include "Core/Engine.h"
#include "Core/Window.h"
#include "Core/WindowManager.h"
#include "Core/Timer.h"

namespace core
{  

  int Engine::Init()
  {    
    m_fTargetFPS = 60.f;

    // Init subsystems
    WindowManager::GetInstance()->Init();

    return 0;
  }

  void Engine::Run()
  {
    m_bRunning = true;

    long long ullMinFrameTime = (long long)(1000.f / m_fTargetFPS);
    Timer timer;

    while (m_bRunning)
    {
      WindowManager::GetInstance()->Update();
      if (ShouldShutDown())
      {
        ShutDown();
      }
      else
      {
        for (const core::Window* pWindow : WindowManager::GetInstance()->GetWindows())
        {
          pWindow->SetCurrent();

          // Render here!

          pWindow->SwapBuffers();
          core::WindowManager::GetInstance()->PollEvents();
        }
      }

      long long elapsed = timer.Peek();
      long long to_sleep = ullMinFrameTime - elapsed;
      to_sleep = to_sleep < 0 ? 0 : to_sleep;
      std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));
      timer.Mark();

      m_fDt = (float)(elapsed + to_sleep) * 0.001f;
      m_fGameTime += m_fDt; 
    }

    // Shut down subsystems
    WindowManager::GetInstance()->ShutDown();

  }

  int Engine::ShutDown()
  {    
    m_bRunning = false;
    return 0;
  }

  bool Engine::ShouldShutDown()
  {
    return WindowManager::GetInstance()->GetWindows().size() == 0;
  }

}