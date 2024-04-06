#pragma once

#include <memory>
 
class Window
{    

  friend class Renderer;  

  struct ConstructKey {};

public:        

  Window(int _iWidth, int _iHeight, const char* _sTitle, ConstructKey&&);
  ~Window();      

  uint8_t GetId() const;

  void SetUsing() const;

private:          

  int BeginDraw() const;
  void EndDraw() const;

  void PollEvents() const;  
  void SwapBuffers() const;

  bool ShouldClose() const;

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
};

