#include "Core/InputManager.h""

#include <GLFW/glfw3.h>

namespace core
{
  void InputManager::PollEvents()
  {
    glfwPollEvents();
  }
}