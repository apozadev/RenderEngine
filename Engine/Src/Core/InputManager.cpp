#include "Core/InputManager.h""

#include <GLFW/glfw3.h>

#include "Graphics/API/GraphicsAPI.h"

void InputManager::PollEvents()
{
  glfwPollEvents();

  api::ImGuiNewFrame();
}
