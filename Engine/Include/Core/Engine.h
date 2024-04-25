#pragma once

#include <vector>

#include "Singleton.h"

#include "Core/Scene.h"
#include "Graphics/Window.h"

class Mesh;

class Engine : public Singleton<Engine>
{

public:
  int Initialize();
  int Run();
  int ScheduleShutDown(); 

  Window* CreateNewWindow(int _iWidth, int _iHeight, const char* _sTitle);
  const std::vector<Window>& GetWindows() { return m_lstWindows; }

  Scene* CreateScene(Window* _pWindow);

  void UpdateWindows();

  bool IsRunning() { return m_bRunning; }

private:

  bool ShouldShutDown();
  void ShutDown();

  bool m_bRunning;

  float m_fTargetFPS;
  float m_fGameTime;
  float m_fDt;    

  std::vector<Scene> m_lstScenes;
  std::vector<Window> m_lstWindows;
};
