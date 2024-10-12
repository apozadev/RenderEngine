#pragma once

#include <memory>

#include <glm/mat4x4.hpp>

struct GLFWwindow;
 
class Window
{

  friend class Engine;  

public:
  
  Window(int _iWidth, int _iHeight, const char* _sTitle);
  Window(Window&& _rWindow);
  ~Window();      

  uint8_t GetId() const;

  int BeginDraw();
  void EndDraw() const;  

  void PollEvents() const;
  void SwapBuffers() const;

  bool ShouldClose() const;

  void SetUsing() const;  

  int GetWidth() const;
  int GetHeight() const;

  void BindDefaultRenderTarget() const;

  uint32_t GetMsaaSamples() const;

  bool IsKeyPressed(char _cKeyCode) const;
  bool IsMousePressed(int _iButton) const;
  void GetMousePos(double& iX_, double& iY_) const;

private:            

  Window& operator=(Window&& _rWindow) noexcept;  

  friend void OnWindowResize(GLFWwindow* _pGflwWindow, int /*width*/, int /*height*/);

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};

