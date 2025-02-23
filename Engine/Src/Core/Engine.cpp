#pragma once

#include <thread>

#include <algorithm>

#include "Core/Engine.h"
#include "Core/Timer.h"
#include "Core/Exception.h"
#include "Core/InputManager.h"
#include "Core/SceneLoading.h"
#include "Graphics/Renderer.h"
#include "Graphics/MaterialLibrary.h"
#include "Memory/Factory.h"

int Engine::Initialize(int _iWidth, int _iHeight, const char* _sTitle)
{    
  m_fTargetFPS = 120.f;

  Factory::Init(1024*1024, 1024*1024);

  // Init subsystems
  Renderer::GetInstance()->Initialize();
  InputManager::GetInstance()->Initialize();

  m_pWindow = std::make_unique<Window>(_iWidth, _iHeight, _sTitle);

  m_pWindow->SetUsing();

  Renderer::GetInstance()->InitializePostWindow();

  m_pfPostUpdateCallback = nullptr;

  return 0;
}

Scene* Engine::CreateScene()
{
  m_lstScenes.emplace_back();  
  return &m_lstScenes.back();
}

int Engine::Run()
{

  Renderer::GetInstance()->Setup();

  m_bRunning = true;

  long long llMinFrameTime = (long long)(1000.f / m_fTargetFPS);
  Timer oTimer;

  while (m_bRunning)
  {
    if (m_pWindow->ShouldClose())
    {
      ScheduleShutDown();
    }
    else
    {  

      if (!m_sScheduledSceneFilename.empty())
      {
        m_lstScenes.clear();        
        MaterialLibrary::GetInstance()->Clear();
        LoadScene(m_sScheduledSceneFilename.c_str(), *CreateScene());
        m_sScheduledSceneFilename.clear();
      }

      long long llElapsed = oTimer.Peek() - llMinFrameTime;

      if (llElapsed > llMinFrameTime)
      {
        oTimer.Mark();

        m_fDt = (float)llMinFrameTime * 0.001f;//(float)llElapsed * 0.001f;
        m_fGameTime += m_fDt;

        InputManager::GetInstance()->PollEvents();

        for (Scene& rScene : m_lstScenes)
        {
          rScene.Update(m_fDt);
        }

        if (m_pfPostUpdateCallback != nullptr)
        {
          m_pfPostUpdateCallback(m_fDt);
        }

        Renderer::GetInstance()->Draw();
      }
    }      
  }    

  ShutDown();

  return 0;    

}

int Engine::ScheduleShutDown()
{    
  m_bRunning = false;
  return 0;
}

void Engine::ShutDown()
{
  // Shut down subsystems      
  InputManager::GetInstance()->ShutDown();  
  MaterialLibrary::GetInstance()->Clear();

  m_lstScenes.clear();  

  Renderer::GetInstance()->ShutDownPreWindow();

  m_pWindow.reset();

  Renderer::GetInstance()->ShutDownPostWindow();
  
}

