#include <thread>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Window.h"
#include "Core/WindowManager.h"
#include "Core/Timer.h"
#include "Core/Exception.h"
#include "Graphics/Renderer.h"

namespace core
{  

  int Engine::Initialize()
  {    
    m_fTargetFPS = 60.f;

    // Init subsystems
    WindowManager::GetInstance()->Initialize();
    Renderer::GetInstance()->Initialize();

    return 0;
  }

  int Engine::Run()
  {

      m_bRunning = true;

      long long ullMinFrameTime = (long long)(1000.f / m_fTargetFPS);
      Timer oTimer;

      while (m_bRunning)
      {
        WindowManager::GetInstance()->Update();
        if (ShouldShutDown())
        {
          ScheduleShutDown();
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

        long long ullElapsed = oTimer.Peek();
        long long ullToSleep = ullMinFrameTime - ullElapsed;
        ullToSleep = ullToSleep < 0 ? 0 : ullToSleep;
        std::this_thread::sleep_for(std::chrono::milliseconds(ullToSleep));
        oTimer.Mark();

        m_fDt = (float)(ullElapsed + ullToSleep) * 0.001f;
        m_fGameTime += m_fDt;
      }    

      ShutDown();

      return 0;    

  }

  int Engine::ScheduleShutDown()
  {    
    m_bRunning = false;
    return 0;
  }

  bool Engine::ShouldShutDown()
  {
    return WindowManager::GetInstance()->GetWindows().size() == 0;
  }

  void Engine::ShutDown()
  {
    // Shut down subsystems
    WindowManager::GetInstance()->ShutDown();
    Renderer::GetInstance()->ShutDown();
  }

}