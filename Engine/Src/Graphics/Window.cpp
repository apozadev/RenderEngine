//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


#include "Core/Exception.h"
#include "Graphics/Window.h"
#include "Graphics/API/GraphicsAPI.h"

class Window::Impl
{
public:
  GLFWwindow* m_pGlfwWindow;
  int m_iWidth, m_iHeight;    
  api::APIWindow* m_pAPIWindow;
  uint8_t m_uId;
};    

uint8_t s_uCurrId = 0u;

Window::Window(int _fWidth, int _fHeight, const char* _sTitle, ConstructKey&&)
{
  m_pImpl = std::make_unique<Impl>();

  m_pImpl->m_pGlfwWindow = nullptr;
  m_pImpl->m_iWidth = _fWidth;
  m_pImpl->m_iHeight = _fHeight;
  m_pImpl->m_uId = s_uCurrId++;  

  if (!glfwInit())
  {
    THROW_GENERIC_EXCEPTION("Could not initialize GLFW")
    return;
  }

  m_pImpl->m_pGlfwWindow = glfwCreateWindow(_fWidth, _fHeight, _sTitle, NULL, NULL);
  if (!m_pImpl->m_pGlfwWindow)
  {
    THROW_GENERIC_EXCEPTION("Could not create GLFW window")
  }        

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

uint8_t Window::GetId() const
{
  return m_pImpl->m_uId;
}

int Window::BeginDraw() const
{
  return api::BeginDraw(m_pImpl->m_pAPIWindow);
}

void Window::EndDraw() const
{
  api::EndDraw(m_pImpl->m_pAPIWindow);
}

void Window::PollEvents() const 
{        
  //glfwMakeContextCurrent(m_pImpl->m_pGlfwWindow);
  glfwPollEvents();
}

void Window::SetUsing() const
{
  api::SetUsingAPIWindow(m_pImpl->m_pAPIWindow);
}

void Window::SwapBuffers() const 
{
  //glfwSwapBuffers(m_pImpl->m_pGlfwWindow);
}

bool Window::ShouldClose() const 
{
  return glfwWindowShouldClose(m_pImpl->m_pGlfwWindow);
}
  