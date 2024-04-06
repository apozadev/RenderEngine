//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


#include "Core/Exception.h"
#include "Graphics/Window.h"
#include "Graphics/API/GraphicsAPI.h"

uint8_t s_uCurrId = 0u;

class Window::Impl
{
public:
  GLFWwindow* m_pGlfwWindow;
  int m_iWidth, m_iHeight;    
  api::APIWindow* m_pAPIWindow;
  uint8_t m_uId;

  Impl(int _fWidth, int _fHeight, const char* _sTitle)
  {
    m_pGlfwWindow = nullptr;
    m_iWidth = _fWidth;
    m_iHeight = _fHeight;
    m_uId = s_uCurrId++;

    if (!glfwInit())
    {
      THROW_GENERIC_EXCEPTION("Could not initialize GLFW")
        return;
    }

    m_pGlfwWindow = glfwCreateWindow(_fWidth, _fHeight, _sTitle, NULL, NULL);

    if (!m_pGlfwWindow)
    {
      THROW_GENERIC_EXCEPTION("Could not create GLFW window")
    }

    m_pAPIWindow = api::CreateAPIWindow(m_pGlfwWindow);

    glfwSetWindowUserPointer(m_pGlfwWindow, static_cast<void*>(m_pAPIWindow));
    glfwSetFramebufferSizeCallback(m_pGlfwWindow, [](GLFWwindow* _pGflwWindow, int /*width*/, int /*height*/)
      {
        api::APIWindow* pAPIWindow = static_cast<api::APIWindow*>(glfwGetWindowUserPointer(_pGflwWindow));
        api::OnWindowResize(pAPIWindow);
      });
  }

  ~Impl()
  {
    api::DestroyAPIWindow(m_pAPIWindow);
    glfwDestroyWindow(m_pGlfwWindow);
  }
};    

Window::Window(int _fWidth, int _fHeight, const char* _sTitle, ConstructKey&&)
{ 
  m_pImpl = std::make_unique<Impl>(_fWidth, _fHeight, _sTitle);    
}

Window::~Window()
{  
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
  