#pragma once

#include "Core/Singleton.h"

namespace core
{
  class Window;

  class InputManager : public Singleton<InputManager>
  {
  public:
    void Initialize() {}
    void ShutDown() {}
    
    void PollEvents();
  };
}