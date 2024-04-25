#pragma once

#include "Core/Singleton.h"

class InputManager : public Singleton<InputManager>
{
public:
  void Initialize() {}
  void ShutDown() {}
    
  void PollEvents();
};
