#pragma once

struct GLFWwindow;

namespace api
{
namespace vk
{

	struct APIWindow;

	void InitializeAPI();

	void ShutDownAPI();

	APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);

	void DrawWindow(APIWindow* _pWindow);

	void OnWindowResize(APIWindow* _pWindow);

	void DestroyAPIWindow(APIWindow* _pAPIWindow);

}
}