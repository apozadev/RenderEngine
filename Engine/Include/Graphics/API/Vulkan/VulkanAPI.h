#pragma once

#include <inttypes.h>

struct GLFWwindow;

namespace api
{
namespace vk
{

	struct APIWindow;
	struct APIMesh;
	struct APIConstantBuffer;

	void InitializeAPI();

	void ShutDownAPI();

	APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);	

	void SetUsingAPIWindow(APIWindow* _pWindow);

	void OnWindowResize(APIWindow* _pWindow);

	void DestroyAPIWindow(APIWindow* _pAPIWindow);

	APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize);

	void DestroyAPIMesh(APIMesh* _pMesh);
	
	APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize);

	void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize);

	void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer);

	void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer);

	int BeginDraw(APIWindow* _pWindow);
	void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount);
	void EndDraw(APIWindow* _pWindow);

}
}