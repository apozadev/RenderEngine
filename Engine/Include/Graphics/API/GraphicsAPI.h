#pragma once

#include <inttypes.h>

#include "Graphics/ImageFormat.h"
#include "Graphics/TextureUsage.h"
#include "Graphics/ResourceBindInfo.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/ConstantBufferTypes.h"
#include "Graphics/SamplerConfig.h"
#include "Graphics/TextureSubResource.h"

#if defined RENDER_API_VULKAN
#define API vk
#elif defined RENDER_API_DX11
#define API dx11
#else
#error "No RENDER_API specified"
#endif

struct GLFWwindow;

namespace api 
{ 

  namespace API 
  { 
    struct APIWindow; 
    struct APIMesh;
    struct APIConstantBuffer;
    struct APIRenderState;
    struct APIRenderSubState;
    struct APITexture;
    struct APIRenderTarget;
  } 

  typedef struct API::APIWindow APIWindow;
  typedef struct API::APIMesh APIMesh;
  typedef struct API::APIConstantBuffer APIConstantBuffer;
  typedef struct API::APIRenderState APIRenderState;
  typedef struct API::APIRenderSubState APIRenderSubState;
  typedef struct API::APITexture APITexture;
  typedef struct API::APIRenderTarget APIRenderTarget;

	// General

	void InitializeAPI();

	void ShutDownAPI();

	uint32_t GetMaxMsaaSamples();

	// Window

	APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow, uint32_t _uMsaaSamples);

	void OnWindowResize(APIWindow* _pWindow);

	uint32_t GetWindowWidth(APIWindow* _pWindow);

	uint32_t GetWindowHeight(APIWindow* _pWindow);

	uint32_t GetWindowMSAASamples(APIWindow* _pWindow);

	void ClearDefaultRenderTarget(APIWindow* _pWindow);

	void BindDefaultRenderTarget(APIWindow* _pWindow);

	void UnbindDefaultRenderTarget(APIWindow* _pWindow);

	void DestroyAPIWindow(APIWindow* _pAPIWindow);

	// Mesh

	APIMesh* CreateAPIMesh(const APIWindow* _pWindow, const void* _pVertexData, size_t _uVertexDataSize, const void* _pIndexData, size_t _uIndexDataSize);

	void DestroyAPIMesh(const APIWindow* _pWindow, APIMesh* _pMesh);

	// Constant buffer

	APIConstantBuffer* CreateAPIConstantBuffer(const APIWindow* _pWindow, size_t _uSize);

	void UpdateAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize);

	void BindAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCbuffer);

	void DestroyAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer);

	// Texture

	APITexture* CreateAPITexture(const APIWindow* _pWindow, const void* const* _ppData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage, const SamplerConfig& _rSamplerConfig, bool _bIsCubemap);

	void GenerateMipMaps(const APIWindow* _pWindow, APITexture* _pTexture);

	void BindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture);

	void UnbindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture);

	void ClearAPITexture(const APIWindow* _pWindow, APITexture* _pTexture, TextureUsage _eUsage);

	void BlitAPITexture(const APIWindow* _pWindow, APITexture* _pSrcTexture, const TextureSubResource& _rSrcSubResource, APITexture* _pDstTexture, const TextureSubResource& _rDstSubResource);

	void DestroyAPITexture(const APIWindow* _pWindow, APITexture* _pTexture);

	// RenderTarget

	APIRenderTarget* CreateAPIRenderTarget();

	void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples, bool _bIsCubemap);

	void RenderTargetAddColorTexture(APITexture* _pTexture, uint32_t _uMipLevel);

	void RenderTargetSetDepthStencilTexture(APITexture* _pTexture, uint32_t _uMipLevel);

	void RenderTargetAddColorResolveTexture(APITexture* _pTexture);

	void EndRenderTargetSetup(const APIWindow* _pWindow);

	void ClearAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget);

	void BindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget);

	void UnbindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget);

	void DestroyAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget);

	// Render state

	APIRenderState* CreateAPIRenderState(const APIWindow* _pWindow, const RenderStateInfo& _oInfo, const APIRenderTarget* _pRenderTarget, uint32_t _uMsaaSamples);

	void BeginRenderStateSetup(APIRenderState* _pAPIRenderState);

	void RenderStateSetupConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState);

	void RenderStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState);

	void EndRenderStateSetup(const APIWindow* _pWindow);

	void BindAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState);

	void DestroyAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState);

	// Shader Reflection

	uint32_t GetConstantBufferCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage);

	uint32_t GetTextureCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage);

	std::string GetConstantBufferName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx);

	const char* GetTextureName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx);

	uint32_t GetConstantBufferMemberCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx);

	std::string GetConstantBufferMemberName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx);

	size_t GetConstantBufferMemberSize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx);

	ConstantBufferType GetConstantBufferMemberType(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx);

	uint32_t GetConstantBufferMemberArraySize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx);

	// Render substate

	APIRenderSubState* CreateAPIRenderSubState(const APIWindow* _pWindow, ResourceFrequency _eFrequency);

	void BeginSubStateSetup(const APIWindow* _pWindow, APIRenderSubState* _pAPIRenderState, ResourceFrequency _eFrequency);

	void SubStateSetupConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo);

	void SubStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo);

	void EndSubStateSetup(const APIWindow* _pWindow);

	void BindAPIRenderSubState(const APIWindow* _pWindow, APIRenderState* _pRenderState, APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency);

	void DestroyRenderSubState(const APIWindow* _pWindow, APIRenderSubState* _pAPIRenderSubState);

	// Drawing

	void WaitForLastImage(const APIWindow* _pWindow);

	int BeginDraw(APIWindow* _pWindow);

	void BeginDrawOffline(APIWindow* _pWindow);

	void DrawMesh(const APIWindow* _pWindow, APIRenderState* _pRenderState, APIMesh* _pMesh, uint32_t _uIndexCount, const void* _pConstantData, uint32_t _uConstantSize);

	void EndDraw(APIWindow* _pWindow);

	void Present(APIWindow* _pWindow);

	// Misc

	void ImGuiNewFrame();

}