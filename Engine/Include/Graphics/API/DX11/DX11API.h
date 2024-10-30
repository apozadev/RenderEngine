#pragma once

#include <inttypes.h>

#include "Graphics/TextureUsage.h"
#include "Graphics/ImageFormat.h"

#include "Graphics/ResourceBindInfo.h"
#include "Graphics/RenderStateInfo.h"

struct GLFWwindow;

namespace api
{
	namespace dx11
	{

		struct APIWindow;
		struct APICamera;
		struct APIMesh;
		struct APIConstantBuffer;
		struct APIRenderState;
		struct APIRenderSubState;
		struct APITexture;
		struct APIRenderTarget;

		// General

		void InitializeAPI();

		void ShutDownAPI();

		uint32_t GetDefaultMsaaSamples();

		// Window

		APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);

		void SetUsingAPIWindow(APIWindow* _pWindow);

		void OnWindowResize(APIWindow* _pWindow);

		uint32_t GetWindowWidth(APIWindow* _pWindow);

		uint32_t GetWindowHeight(APIWindow* _pWindow);

		void ClearDefaultRenderTarget(APIWindow* _pWindow);

		void BindDefaultRenderTarget(APIWindow* _pWindow);

		void UnbindDefaultRenderTarget(APIWindow* _pWindow);

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

		APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage);

		void BindAPITexture(APITexture* _pTexture);

		void ClearAPITexture(APITexture* _pTexture, TextureUsage _eUsage);

		void DestroyAPITexture(APITexture* _pTexture);

		// RenderTarget

		APIRenderTarget* CreateAPIRenderTarget();

		void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples);

		void RenderTargetAddColorTexture(APITexture* _pTexture);

		void RenderTargetSetDepthStencilTexture(APITexture* _pTexture);

		void RenderTargetAddColorResolveTexture(APITexture* _pTexture);

		void EndRenderTargetSetup();

		void ClearAPIRenderTarget(APIRenderTarget* _pRenderTarget);

		void BindAPIRenderTarget(APIRenderTarget* _pRenderTarget);

		void SetUsingAPIRenderTarget(APIRenderTarget* _pRenderTarget);

		void UnbindAPIRenderTarget(APIRenderTarget* _pRenderTarget);

		void DestroyAPIRenderTarget(APIRenderTarget* _pRenderTarget);

		// Render state

		APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo, uint32_t _uMsaaSamples);

		void BeginRenderStateSetup(APIRenderState* _pAPIRenderState);

		void EndRenderStateSetup();

		void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState);

		void BindAPIRenderState(APIRenderState* _pAPIRenderState);

		void DestroyAPIRenderState(APIRenderState* _pAPIRenderState);

		// Render substate

		APIRenderSubState* CreateAPIRenderSubState(ResourceFrequency _eFrequency);

		void BeginSubStateSetup(APIRenderSubState* _pAPIRenderState);

		void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo);

		void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo);

		void EndSubStateSetup(ResourceFrequency _eFrequency);

		void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency);

		void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState);

		// Drawing

		int BeginDraw(APIWindow* _pWindow);

		void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount, void* _pConstantData, uint32_t _uConstantSize);

		void EndDraw(APIWindow* _pWindow);

	}
}