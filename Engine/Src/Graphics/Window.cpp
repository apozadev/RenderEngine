//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Core/Exception.h"
#include "Graphics/Renderer.h"
#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"
#include "Util/KeyboardUtil.h"

namespace window_internal
{
  uint8_t s_uCurrId = 0u;
}

Window::Window(int _fWidth, int _fHeight, const char* _sTitle)
{   
  m_pGlfwWindow = nullptr;
  m_uId = window_internal::s_uCurrId++;

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
}

Window::~Window()
{  
  api::DestroyAPIWindow(m_pAPIWindow);
  glfwDestroyWindow(m_pGlfwWindow);
}

uint8_t Window::GetId() const
{
  return m_uId;
}

int Window::BeginDraw() 
{ 

  glfwSetWindowUserPointer(m_pGlfwWindow, static_cast<void*>(this));
  glfwSetFramebufferSizeCallback(m_pGlfwWindow, OnWindowResize);

  int iResult = api::BeginDraw(m_pAPIWindow);

  if (iResult == 0)
  {
    api::ClearDefaultRenderTarget(m_pAPIWindow);
  }  

  return iResult;
}

void Window::EndDraw() const
{
  api::EndDraw(m_pAPIWindow);
}

void Window::PollEvents() const 
{        
  //glfwMakeContextCurrent(m_pGlfwWindow);
  glfwPollEvents();
}

void Window::SetUsing() const
{
  api::SetUsingAPIWindow(m_pAPIWindow);
}

void Window::SwapBuffers() const 
{
  //glfwSwapBuffers(m_pGlfwWindow);
}

int Window::GetWidth() const
{
  return api::GetWindowWidth(m_pAPIWindow);
}

int Window::GetHeight() const
{
  return api::GetWindowHeight(m_pAPIWindow);
}

void Window::BindDefaultRenderTarget() const
{
  api::BindDefaultRenderTarget(m_pAPIWindow);
}

void Window::UnbindDefaultRenderTarget() const
{
  api::UnbindDefaultRenderTarget(m_pAPIWindow);
}

uint32_t Window::GetMsaaSamples() const
{
  return api::GetDefaultMsaaSamples();
}

bool Window::ShouldClose() const 
{
  return glfwWindowShouldClose(m_pGlfwWindow);
}

bool Window::IsKeyPressed(char _cKeyCode) const
{ 
  return glfwGetKey(m_pGlfwWindow, GetCharKeyCode(_cKeyCode)) == GLFW_PRESS;
}

bool Window::IsMousePressed(int _iButton) const
{
  return glfwGetMouseButton(m_pGlfwWindow, _iButton) == GLFW_PRESS;
}

void Window::GetMousePos(double& iX_, double& iY_) const
{
  glfwGetCursorPos(m_pGlfwWindow, &iX_, &iY_);
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
  api::OnWindowResize(pWindow->m_pAPIWindow);  
}
