#pragma once

#include <inttypes.h>

struct GLFWwindow;

namespace api
{
namespace vk
{

	struct APIWindow;
	struct APIMesh;

	void InitializeAPI();

	void ShutDownAPI();

	APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);	

	void SetUsingAPIWindow(APIWindow* _pWindow);

	void OnWindowResize(APIWindow* _pWindow);

	void DestroyAPIWindow(APIWindow* _pAPIWindow);

	APIMesh* CreateAPIMesh(void* _pData, size_t _uSize);

	void DestroyAPIMesh(APIMesh* _pMesh);

	int BeginDraw(APIWindow* _pWindow);
	void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount);
	void EndDraw(APIWindow* _pWindow);

}
}