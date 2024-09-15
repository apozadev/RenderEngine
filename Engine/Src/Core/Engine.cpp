#pragma once

#include <thread>

#include <algorithm>

#include "Core/Engine.h"
#include "Core/Timer.h"
#include "Core/Exception.h"
#include "Core/InputManager.h"
#include "Graphics/Renderer.h"
#include "Graphics/MaterialLibrary.h"

int Engine::Initialize()
{    
  m_fTargetFPS = 120.f;

  // Init subsystems
  Renderer::GetInstance()->Initialize();
  InputManager::GetInstance()->Initialize();

  return 0;
}

Window* Engine::CreateNewWindow(int _iWidth, int _iHeight, const char* _sTitle)
{      
  m_lstWindows.push_back(Window(_iWidth, _iHeight, _sTitle));
  return &m_lstWindows.back();
}

Scene* Engine::CreateScene(Window* _pWindow)
{
  m_lstScenes.push_back(std::move(Scene(_pWindow)));  
  return &m_lstScenes.back();
}

void Engine::UpdateWindows()
{
  size_t uCurrSize = m_lstWindows.size();
  for (int i = 0; i < uCurrSize; i++)
  {
    if (m_lstWindows[i].ShouldClose())
    {

      Window& rWindow = m_lstWindows[i];

      for (int j = 0; j < m_lstScenes.size(); j++)
      {        
        if (m_lstScenes[j].GetWindow() == &rWindow)
        {
          if (j < m_lstScenes.size() - 1)
          {
            Scene oAux(std::move(m_lstScenes[j]));
            m_lstScenes[j] = std::move(m_lstScenes[m_lstScenes.size() - 1]);
            m_lstScenes[m_lstScenes.size() - 1] = std::move(oAux);
          }
          m_lstScenes.pop_back();
        }
      }

      MaterialLibrary::GetInstance()->DestroyWindowMaterials(&rWindow);

      if (i < uCurrSize - 1)
      {        
        Window oAux(std::move(m_lstWindows[i]));
        m_lstWindows[i] = std::move(m_lstWindows[m_lstWindows.size() - 1]);
        m_lstWindows[m_lstWindows.size() - 1] = std::move(oAux);
      }      

      m_lstWindows.pop_back();
    }
  }    
}

int Engine::Run()
{

    m_bRunning = true;

    long long llMinFrameTime = (long long)(1000.f / m_fTargetFPS);
    Timer oTimer;

    while (m_bRunning)
    {
      if (ShouldShutDown())
      {
        ScheduleShutDown();
      }
      else
      {  
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

          Renderer::GetInstance()->Draw();

          UpdateWindows();
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

bool Engine::ShouldShutDown()
{
  return m_lstWindows.size() == 0;
}

void Engine::ShutDown()
{
  // Shut down subsystems      
  InputManager::GetInstance()->ShutDown();
  Renderer::GetInstance()->ShutDown();
}

