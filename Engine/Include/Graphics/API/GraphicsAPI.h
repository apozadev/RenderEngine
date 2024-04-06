#pragma once

#include <inttypes.h>

struct GLFWwindow;

namespace api 
{ 

  namespace RENDER_API 
  { 
    struct APIWindow; 
    struct APIMesh;
    struct APIConstantBuffer;
    struct APIRenderState;
  } 

  typedef struct RENDER_API::APIWindow APIWindow;
  typedef struct RENDER_API::APIMesh APIMesh;
  typedef struct RENDER_API::APIConstantBuffer APIConstantBuffer;
  typedef struct RENDER_API::APIRenderState APIRenderState;

  // Global

  void InitializeAPI();

  void ShutDownAPI();

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);
  void SetUsingAPIWindow(APIWindow* _pWindow);
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
  void BindAPIRenderState(APIRenderState* _pAPIRenderState);
  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState);
  void SetUsingRenderState(APIRenderState* _pRenderState);

  // Drawing

  int BeginDraw(APIWindow* _pWindow);
  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount);
  void EndDraw(APIWindow* _pWindow);    

}