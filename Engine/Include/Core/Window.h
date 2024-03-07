#pragma once

#include <memory>

namespace core
{  
  class Window
  {    

    friend class WindowManager;

    struct ConstructKey {};

  public:        

    ~Window();    

    void SetCurrent() const;
    void SwapBuffers() const;

    void Draw() const; 

    bool ShouldClose() const;    

  private:        
    
    Window(int _iWidth, int _iHeight, const char* _sTitle, ConstructKey&&);    

    class Impl;
    std::unique_ptr<Impl> m_pImpl;
  };
}
