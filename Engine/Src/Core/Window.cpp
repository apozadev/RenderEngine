//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Core/Window.h"
#include "Graphics/API/GraphicsAPI.h"

namespace core {

  class Window::Impl
  {
  public:
    GLFWwindow* m_pGlfwWindow;
    int m_iWidth, m_iHeight;    
    api::APIWindow* m_pAPIWindow;
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

    //api::APIWindow* pAPIWindow = ;
    m_pImpl->m_pAPIWindow = api::CreateAPIWindow(m_pImpl->m_pGlfwWindow);

    glfwSetWindowUserPointer(m_pImpl->m_pGlfwWindow, static_cast<void*>(m_pImpl->m_pAPIWindow));
    glfwSetFramebufferSizeCallback(m_pImpl->m_pGlfwWindow, [](GLFWwindow* _pGflwWindow, int /*width*/, int /*height*/)
      {
        api::APIWindow* pAPIWindow = static_cast<api::APIWindow*>(glfwGetWindowUserPointer(_pGflwWindow));
        api::OnWindowResize(pAPIWindow);
      });

  }

  Window::~Window()
  {
    api::DestroyAPIWindow(m_pImpl->m_pAPIWindow);
    glfwDestroyWindow(m_pImpl->m_pGlfwWindow);
  }

  void Window::SetCurrent() const 
  {        
    glfwMakeContextCurrent(m_pImpl->m_pGlfwWindow);
  }

  void Window::SwapBuffers() const 
  {
    //glfwSwapBuffers(m_pImpl->m_pGlfwWindow);
  }

  void Window::Draw() const
  {
    api::DrawWindow(m_pImpl->m_pAPIWindow);
  }

  bool Window::ShouldClose() const 
  {
    return glfwWindowShouldClose(m_pImpl->m_pGlfwWindow);
  }
  
}
