#pragma once

struct GLFWwindow;

namespace api 
{ 

  namespace RENDER_API { struct APIWindow; } 

  typedef struct RENDER_API::APIWindow APIWindow;

  void InitializeAPI();

  void ShutDownAPI();

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow);

  void DrawWindow(APIWindow* _pWindow);

  void OnWindowResize(APIWindow* _pWindow);

  void DestroyAPIWindow(APIWindow* _pWindow);

}