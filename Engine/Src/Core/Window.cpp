//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Core/Window.h"

namespace core {

  class Window::Impl
  {
  public:
    GLFWwindow* m_pGlfwWindow;
    int m_iWidth, m_iHeight;    
  };    

  Window::Window(int _fWidth, int _fHeight, const char* _sTitle, ConstructKey&&)
  {
    m_pImpl = std::make_unique<Impl>();

    m_pImpl->m_pGlfwWindow = nullptr;
    m_pImpl->m_iWidth = _fWidth;
    m_pImpl->m_iHeight = _fHeight;

    if (!glfwInit())
    {
      // TODO: [ERROR] Could not initialize GLFW
      return;
    }

    m_pImpl->m_pGlfwWindow = glfwCreateWindow(_fWidth, _fHeight, _sTitle, NULL, NULL);
    if (!m_pImpl->m_pGlfwWindow)
    {
      // TODO: [ERROR] Could not create GLFW window
    }

  }

  Window::~Window()
  {

  }

  void Window::SetCurrent() const 
  {
    glfwMakeContextCurrent(m_pImpl->m_pGlfwWindow);
  }

  void Window::SwapBuffers() const 
  {
    glfwSwapBuffers(m_pImpl->m_pGlfwWindow);
  }

  bool Window::ShouldClose() const 
  {
    return glfwWindowShouldClose(m_pImpl->m_pGlfwWindow);
  }
  
}
