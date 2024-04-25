#pragma once

#include <memory>
 
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

  void PollEvents() const;
  void SwapBuffers() const;

  bool ShouldClose() const;

  void SetUsing() const;

private:            

  Window& operator=(Window&& _rWindow) noexcept;

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};

