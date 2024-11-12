#pragma once

#include <memory>

#include <glm/mat4x4.hpp>

#include "Graphics/API/GraphicsAPI.h"

struct GLFWwindow;
 
class Window
{

  friend class Engine;  

public:
  
  Window(int _iWidth, int _iHeight, const char* _sTitle);
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
  void UnbindDefaultRenderTarget() const;

  uint32_t GetMsaaSamples() const;

  bool IsKeyPressed(char _cKeyCode) const;
  bool IsMousePressed(int _iButton) const;
  void GetMousePos(double& iX_, double& iY_) const;

private:              

  friend void OnWindowResize(GLFWwindow* _pGflwWindow, int /*width*/, int /*height*/);

private:  

  GLFWwindow* m_pGlfwWindow;

  api::APIWindow* m_pAPIWindow;

  uint8_t m_uId;

};

