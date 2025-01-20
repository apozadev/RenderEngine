#pragma once

#include <vector>
#include <memory>

#include "Singleton.h"

#include "Core/Scene.h"
#include "Graphics/Window.h"

class Mesh;

class Engine : public Singleton<Engine>
{

public:
  int Initialize(int _iWidth, int _iHeight, const char* _sTitle);
  int Run();
  int ScheduleShutDown(); 

  void SetPostUpdateCallback(void (*_pfPostUpdateCallback) (float)) { m_pfPostUpdateCallback = _pfPostUpdateCallback; }
  
  Window* GetWindow() const { return m_pWindow.get(); }

  Scene* CreateScene();  

  std::vector<Scene>& GetScenes() { return m_lstScenes; }

  bool IsRunning() { return m_bRunning; }

private:

  void ShutDown();

  bool m_bRunning;

  float m_fTargetFPS;
  float m_fGameTime;
  float m_fDt;    

  void (*m_pfPostUpdateCallback) (float);

  std::vector<Scene> m_lstScenes;
  std::unique_ptr<Window> m_pWindow;
};
