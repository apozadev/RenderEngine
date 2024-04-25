#include "Core/InputManager.h""

#include <GLFW/glfw3.h>

void InputManager::PollEvents()
{
  glfwPollEvents();
}
