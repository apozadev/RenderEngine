#pragma once

#include "Core/Singleton.h"
#include <vector>

namespace core
{
  class Window;

  class WindowManager : public Singleton<WindowManager>
  {
  public:
    void Init();
    void ShutDown();   
    Window* CreateWindow(int _iWidth, int _iHeight, const char* _sTitle);
    const std::vector<Window*>& GetWindows() { return m_lstWindows; }
    void Update();
    void PollEvents();    
  private:
    std::vector<Window*> m_lstWindows;
    core::Window* m_pCurrentWindow;
  };
}