//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Core/Exception.h"
#include "Graphics/Renderer.h"
#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Util/KeyboardUtil.h"

uint8_t s_uCurrId = 0u;

class Window::Impl
{

public:

  GLFWwindow* m_pGlfwWindow;  
  api::APIWindow* m_pAPIWindow;
  uint8_t m_uId;    

  struct APIWindowPair
  {
    Window* m_pWindow;
    api::APIWindow* m_pAPIWindow;
  } m_oWindowPair;

  Impl(int _fWidth, int _fHeight, const char* _sTitle, Window* _pWindow)    
  {
    m_pGlfwWindow = nullptr;    
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

    m_oWindowPair.m_pAPIWindow = m_pAPIWindow;
    m_oWindowPair.m_pWindow = _pWindow;    
  }    

  ~Impl()
  {    
    api::DestroyAPIWindow(m_pAPIWindow);
    glfwDestroyWindow(m_pGlfwWindow);
  }
};    

Window::Window(int _fWidth, int _fHeight, const char* _sTitle)
{ 
  m_pImpl = std::make_unique<Impl>(_fWidth, _fHeight, _sTitle, this);
}

Window::Window(Window&& _rWindow)
{
  m_pImpl = std::move(_rWindow.m_pImpl);
}

Window::~Window()
{  
}

uint8_t Window::GetId() const
{
  return m_pImpl->m_uId;
}

int Window::BeginDraw() 
{ 

  glfwSetWindowUserPointer(m_pImpl->m_pGlfwWindow, static_cast<void*>(this));
  glfwSetFramebufferSizeCallback(m_pImpl->m_pGlfwWindow, OnWindowResize);

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

int Window::GetWidth() const
{
  return api::GetWindowWidth(m_pImpl->m_pAPIWindow);
}

int Window::GetHeight() const
{
  return api::GetWindowHeight(m_pImpl->m_pAPIWindow);
}

void Window::BindDefaultRenderTarget() const
{
  api::BindDefaultRenderTarget(m_pImpl->m_pAPIWindow);
}

uint32_t Window::GetMsaaSamples() const
{
  return api::GetDefaultMsaaSamples();
}

bool Window::ShouldClose() const 
{
  return glfwWindowShouldClose(m_pImpl->m_pGlfwWindow);
}

bool Window::IsKeyPressed(char _cKeyCode) const
{ 
  return glfwGetKey(m_pImpl->m_pGlfwWindow, GetCharKeyCode(_cKeyCode)) == GLFW_PRESS;
}

bool Window::IsMousePressed(int _iButton) const
{
  return glfwGetMouseButton(m_pImpl->m_pGlfwWindow, _iButton) == GLFW_PRESS;
}

void Window::GetMousePos(double& iX_, double& iY_) const
{
  glfwGetCursorPos(m_pImpl->m_pGlfwWindow, &iX_, &iY_);
}
  
Window& Window::operator=(Window&& _rWindow) noexcept
{
  m_pImpl = std::move(_rWindow.m_pImpl);
  return *this;
}

void OnWindowResize(GLFWwindow* _pGflwWindow, int /*width*/, int /*height*/)
{
  int width = 0, height = 0;
  glfwGetFramebufferSize(_pGflwWindow, &width, &height);
  while (width == 0 || height == 0)
  {
    glfwGetFramebufferSize(_pGflwWindow, &width, &height);
    glfwWaitEvents();
  }

  Window* pWindow = static_cast<Window*>(glfwGetWindowUserPointer(_pGflwWindow));
  api::OnWindowResize(pWindow->m_pImpl->m_pAPIWindow);  
}
