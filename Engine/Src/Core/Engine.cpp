#include <thread>

#include <Windows.h>

#include "Core/Engine.h"
#include "Core/Timer.h"
#include "Core/Exception.h"
#include "Core/InputManager.h"

#include "Graphics/Window.h"
#include "Graphics/Renderer.h"

namespace core
{  

  int Engine::Initialize()
  {    
    m_fTargetFPS = 60.f;    

    // Init subsystems
    Renderer::GetInstance()->Initialize();
    InputManager::GetInstance()->Initialize();

    return 0;
  }

  int Engine::Run()
  {

      m_bRunning = true;

      long long ullMinFrameTime = (long long)(1000.f / m_fTargetFPS);
      Timer oTimer;

      while (m_bRunning)
      {
        if (ShouldShutDown())
        {
          ScheduleShutDown();
        }
        else
        {
          InputManager::GetInstance()->PollEvents();

          Renderer::GetInstance()->UpdateWindows();

          for (Mesh* pMesh : m_lstMeshes)
          {
            Renderer::GetInstance()->SubmitMesh(pMesh);
          }

          Renderer::GetInstance()->Draw();
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
    return Renderer::GetInstance()->GetWindows().size() == 0;
  }

  void Engine::ShutDown()
  {
    // Shut down subsystems    
    InputManager::GetInstance()->ShutDown();
    Renderer::GetInstance()->ShutDown();
  }

  void Engine::AddMesh(Mesh* _pMesh)
  {
    m_lstMeshes.push_back(_pMesh);
  }

}