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
	struct APIRenderState;

	// General

	void InitializeAPI();

	void ShutDownAPI();

	// Window

	APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);	

	void SetUsingAPIWindow(APIWindow* _pWindow);

	void OnWindowResize(APIWindow* _pWindow);

	void DestroyAPIWindow(APIWindow* _pAPIWindow);

	// Mesh

	APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize);

	void DestroyAPIMesh(APIMesh* _pMesh);

	// Constant buffer
	
	APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize);

	void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize);

	void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer);

	void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer);

	// Render state

	APIRenderState* CreateAPIRenderState();

	void BindAPIRenderState(APIRenderState* _pAPIRenderState);

	void DestroyAPIRenderState(APIRenderState* _pAPIRenderState);

	void SetUsingRenderState(APIRenderState* _pRenderState);

	// Drawing

	int BeginDraw(APIWindow* _pWindow);
	void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount);
	void EndDraw(APIWindow* _pWindow);

}
}