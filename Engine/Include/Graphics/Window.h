#pragma once

#include <memory>

#include <glm/mat4x4.hpp>

struct GlobalBufferData
{
  glm::mat4 m_mViewProj;
};
 
class Window
{

  friend class Engine;

  //struct ConstructKey {};

public:
  
  Window(int _iWidth, int _iHeight, const char* _sTitle);
  Window(Window&& _rWindow);
  ~Window();      

  uint8_t GetId() const;

  int BeginDraw() const;
  void EndDraw() const;

  void UpdateGlobalBufferData (GlobalBufferData&& _rData) const;

  void PollEvents() const;
  void SwapBuffers() const;

  bool ShouldClose() const;

  void SetUsing() const;

  void Setup() const;

private:            

  Window& operator=(Window&& _rWindow) noexcept;

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};

