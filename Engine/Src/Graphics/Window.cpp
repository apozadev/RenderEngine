//#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Core/Exception.h"
#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"

#include <glm/ext/matrix_clip_space.hpp>

uint8_t s_uCurrId = 0u;

class Window::Impl
{

public:

  GLFWwindow* m_pGlfwWindow;
  int m_iWidth, m_iHeight;    
  api::APIWindow* m_pAPIWindow;
  uint8_t m_uId;
  ConstantBuffer<GlobalBufferData>* m_pGlobalCBuffer;

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

    api::SetUsingAPIWindow(m_pAPIWindow);

    m_pGlobalCBuffer = new ConstantBuffer<GlobalBufferData>(0, PipelineStage::VERTEX);

    api::SetUsingAPIWindow(nullptr);

    glfwSetWindowUserPointer(m_pGlfwWindow, static_cast<void*>(m_pAPIWindow));
    glfwSetFramebufferSizeCallback(m_pGlfwWindow, [](GLFWwindow* _pGflwWindow, int /*width*/, int /*height*/)
    {
      api::APIWindow* pAPIWindow = static_cast<api::APIWindow*>(glfwGetWindowUserPointer(_pGflwWindow));
      api::OnWindowResize(pAPIWindow);
    });
  }

  ~Impl()
  {
    delete m_pGlobalCBuffer;
    api::DestroyAPIWindow(m_pAPIWindow);
    glfwDestroyWindow(m_pGlfwWindow);
  }
};    

Window::Window(int _fWidth, int _fHeight, const char* _sTitle)
{ 
  m_pImpl = std::make_unique<Impl>(_fWidth, _fHeight, _sTitle);
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

int Window::BeginDraw() const
{    
  return api::BeginDraw(m_pImpl->m_pAPIWindow);
}

void Window::EndDraw() const
{
  api::EndDraw(m_pImpl->m_pAPIWindow);
}

void Window::UpdateGlobalBufferData(GlobalBufferData&& /*_rData*/) const
{

  GlobalBufferData oData{};
  oData.m_mViewProj = glm::perspective(45.f, (float)m_pImpl->m_iWidth / m_pImpl->m_iHeight, 0.1f, 100.0f);

  //m_pImpl->m_pGlobalCBuffer->SetData(&_rData);  
  m_pImpl->m_pGlobalCBuffer->SetData(&oData);

  m_pImpl->m_pGlobalCBuffer->Update();

  api::BindWindowSubState(m_pImpl->m_pAPIWindow);
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

void Window::Setup() const
{
  api::BeginWindowSubStateSetup(m_pImpl->m_pAPIWindow);

  m_pImpl->m_pGlobalCBuffer->Setup(ResourceFrequency::GLOBAL);

  api::EndWindowSubStateSetup();
}

void Window::SwapBuffers() const 
{
  //glfwSwapBuffers(m_pImpl->m_pGlfwWindow);
}

bool Window::ShouldClose() const 
{
  return glfwWindowShouldClose(m_pImpl->m_pGlfwWindow);
}
  
Window& Window::operator=(Window&& _rWindow) noexcept
{
  m_pImpl = std::move(_rWindow.m_pImpl);
  return *this;
}
