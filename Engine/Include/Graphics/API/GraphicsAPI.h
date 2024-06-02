#pragma once

#include <inttypes.h>

#include "Graphics/ImageFormat.h"
#include "Graphics/ResourceBindInfo.h"

struct GLFWwindow;

namespace api 
{ 

  namespace RENDER_API 
  { 
    struct APIWindow; 
    struct APIMesh;
    struct APIConstantBuffer;
    struct APIRenderState;
    struct APIRenderSubState;
    struct APITexture;
  } 

  typedef struct RENDER_API::APIWindow APIWindow;
  typedef struct RENDER_API::APIMesh APIMesh;
  typedef struct RENDER_API::APIConstantBuffer APIConstantBuffer;
  typedef struct RENDER_API::APIRenderState APIRenderState;
  typedef struct RENDER_API::APIRenderSubState APIRenderSubState;
  typedef struct RENDER_API::APITexture APITexture;

  // Global

  void InitializeAPI();

  void ShutDownAPI();

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);
  void SetUsingAPIWindow(APIWindow* _pWindow);
  void BeginWindowSubStateSetup(APIWindow* _pWindow);
  void EndWindowSubStateSetup();
  void BindWindowSubState(APIWindow* _pWindow);
  void OnWindowResize(APIWindow* _pWindow);
  void DestroyAPIWindow(APIWindow* _pWindow);

  // Mesh

  APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize);
  void DestroyAPIMesh(APIMesh* _pMesh);

  // ConstantBuffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize);
  void UpdateAPIConstantBuffer(APIConstantBuffer* _pCbuffer, const void* _pData, size_t _uSize);
  void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer);
  void DestroyAPIConstantBuffer(APIConstantBuffer*_pCbuffer);

  // Render state

  APIRenderState* CreateAPIRenderState();
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

  APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat);
  void BindAPITexture(APITexture* _pTexture);
  void DestroyAPITexture(APITexture* _pTexture);

  // Drawing

  int BeginDraw(APIWindow* _pWindow);
  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount);
  void EndDraw(APIWindow* _pWindow);    

}