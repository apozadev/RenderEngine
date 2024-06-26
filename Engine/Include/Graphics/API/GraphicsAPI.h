#pragma once

#include <inttypes.h>

#include "Graphics/ImageFormat.h"
#include "Graphics/ResourceBindInfo.h"
#include "Graphics/RenderStateInfo.h"

#if RENDER_API == 0
#define API vk
#elif RENDER_API == 1
#define API dx11
#else
#error "Unknown RENDER_API specified"
#endif

struct GLFWwindow;

namespace api 
{ 

  namespace API 
  { 
    struct APIWindow; 
    struct APICamera;
    struct APIMesh;
    struct APIConstantBuffer;
    struct APIRenderState;
    struct APIRenderSubState;
    struct APITexture;
  } 

  typedef struct API::APIWindow APIWindow;
  typedef struct API::APICamera APICamera;
  typedef struct API::APIMesh APIMesh;
  typedef struct API::APIConstantBuffer APIConstantBuffer;
  typedef struct API::APIRenderState APIRenderState;
  typedef struct API::APIRenderSubState APIRenderSubState;
  typedef struct API::APITexture APITexture;

  // Global

  void InitializeAPI();

  void ShutDownAPI();

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);
  void SetUsingAPIWindow(APIWindow* _pWindow);
  void OnWindowResize(APIWindow* _pWindow);
  uint32_t GetWindowWidth(APIWindow* _pWindow);
  uint32_t GetWindowHeight(APIWindow* _pWindow);
  void DestroyAPIWindow(APIWindow* _pWindow);

  // Camera

  APICamera* CreateAPICamera();
  void BeginCameraSubStateSetup(APICamera* _pCamera);
  void EndCameraSubstateSetup(APICamera* _pCamera);
  void BindAPICamera(APICamera* _pCamera);
  void DestroyAPICamera(APICamera* _pCamera);

  // Mesh

  APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize);
  void DestroyAPIMesh(APIMesh* _pMesh);

  // ConstantBuffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize);
  void UpdateAPIConstantBuffer(APIConstantBuffer* _pCbuffer, const void* _pData, size_t _uSize);
  void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer);
  void DestroyAPIConstantBuffer(APIConstantBuffer*_pCbuffer);

  // Render state

  APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo);
  void BeginRenderStateSetup(APIRenderState* _pAPIRenderState);
  void EndRenderStateSetup();
  void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState);
  void BindAPIRenderState(APIRenderState* _pAPIRenderState);
  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState);

  // Render substate

  APIRenderSubState* CreateAPIRenderSubState();
  void BeginSubStateSetup(APIRenderSubState* _pAPIRenderSubState);
  void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo);
  void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo);
  void EndSubStateSetup();
  void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState);
  void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState);

  // Texture

  APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels);
  void BindAPITexture(APITexture* _pTexture);
  void DestroyAPITexture(APITexture* _pTexture);

  // Drawing

  int BeginDraw(APIWindow* _pWindow);
  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount, void* _pConstantData, uint32_t _uConstantSize);
  void EndDraw(APIWindow* _pWindow);    

}