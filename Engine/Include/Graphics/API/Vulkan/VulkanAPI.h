#pragma once

#include <inttypes.h>

#include "Graphics/ImageFormat.h"

#include "Graphics/ResourceBindInfo.h"

struct GLFWwindow;

namespace api
{
namespace vk
{

	struct APIWindow;
	struct APICamera;
	struct APIMesh;
	struct APIConstantBuffer;
	struct APIRenderState;
	struct APIRenderSubState;
	struct APITexture;	

	// General

	void InitializeAPI();	

	void ShutDownAPI();

	// Window

	APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);	

	void SetUsingAPIWindow(APIWindow* _pWindow);	

	void OnWindowResize(APIWindow* _pWindow);

	void DestroyAPIWindow(APIWindow* _pAPIWindow);

	// Camera

	APICamera* CreateAPICamera();

	void BeginCameraSubStateSetup(APICamera* _pCamera);

	void EndCameraSubstateSetup(APICamera* _pCamera);

	void BindAPICamera(APICamera* _pCamera);

	void DestroyAPICamera(APICamera* _pCamera);

	// Mesh

	APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize);

	void DestroyAPIMesh(APIMesh* _pMesh);

	// Constant buffer
	
	APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize);

	void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize);

	void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer);

	void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer);

	// Texture

	APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat);

	void BindAPITexture(APITexture* _pTexture);

	void DestroyAPITexture(APITexture* _pTexture);

	// Render state

	APIRenderState* CreateAPIRenderState();			

	void BeginRenderStateSetup(APIRenderState* _pAPIRenderState);

	void EndRenderStateSetup();

	void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState);

	void BindAPIRenderState(APIRenderState* _pAPIRenderState);	

	void DestroyAPIRenderState(APIRenderState* _pAPIRenderState);

	// Render substate

	APIRenderSubState* CreateAPIRenderSubState();

	void BeginSubStateSetup(APIRenderSubState* _pAPIRenderState);

	void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo);

	void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo);

	void EndSubStateSetup();

	void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState);

	void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState);

	// Drawing

	int BeginDraw(APIWindow* _pWindow);
	void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount);
	void EndDraw(APIWindow* _pWindow);

}
}