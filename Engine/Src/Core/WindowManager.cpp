#include "Core/WindowManager.h"
#include "Core/Window.h"

#include <GLFW/glfw3.h>
#include <algorithm>

namespace core
{  
  void WindowManager::Init()
  {
    glfwInit();
  }

  void WindowManager::ShutDown()
  {
    glfwTerminate();
  }

  Window* WindowManager::CreateWindow(int _iWidth, int _iHeight, const char* _sTitle)
  {
    m_lstWindows.push_back(new Window(_iWidth, _iHeight, _sTitle, Window::ConstructKey()));
    return m_lstWindows[m_lstWindows.size() - 1];    
  }
  
  void WindowManager::Update()
  {    
    size_t uCurrSize = m_lstWindows.size();
    for (int i = 0; i < uCurrSize; i++)
    {
      if (m_lstWindows[i]->ShouldClose())
      {
        if (i < uCurrSize - 1)
        {          
          delete(m_lstWindows[i]);
          m_lstWindows[i] = m_lstWindows[m_lstWindows.size() - 1];
        }
        --uCurrSize;
      }
    }
    m_lstWindows.resize(uCurrSize);
  }

  void WindowManager::PollEvents()
  {
    glfwPollEvents();
  }
}