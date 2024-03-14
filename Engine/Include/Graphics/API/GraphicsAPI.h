#pragma once

#include <inttypes.h>

struct GLFWwindow;

namespace api 
{ 

  namespace RENDER_API 
  { 
    struct APIWindow; 
    struct APIMesh;
  } 

  typedef struct RENDER_API::APIWindow APIWindow;
  typedef struct RENDER_API::APIMesh APIMesh;

  // Global

  void InitializeAPI();

  void ShutDownAPI();

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);
  void SetUsingAPIWindow(APIWindow* _pWindow);
  void OnWindowResize(APIWindow* _pWindow);
  void DestroyAPIWindow(APIWindow* _pWindow);

  // Mesh

  APIMesh* CreateAPIMesh(void* _pData, size_t _uSize);

  // Drawing

  int BeginDraw(APIWindow* _pWindow);
  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount);
  void EndDraw(APIWindow* _pWindow);    

}