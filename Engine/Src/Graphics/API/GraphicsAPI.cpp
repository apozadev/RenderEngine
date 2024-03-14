#include "Graphics/API/GraphicsAPI.h"

#if RENDER_API == vk
#include "Graphics/API/Vulkan/VulkanAPI.h"
#endif

namespace api
{    

  // Global

  void InitializeAPI()
  {
    RENDER_API::InitializeAPI();
  }

  void ShutDownAPI()
  {
    RENDER_API::ShutDownAPI();
  } 

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
  {    
    return RENDER_API::CreateAPIWindow(_pGlfwWindow);
  }

  void SetUsingAPIWindow(APIWindow* _pWindow)
  {
    RENDER_API::SetUsingAPIWindow(_pWindow);
  }

  void OnWindowResize(APIWindow* _pWindow)
  {
    RENDER_API::OnWindowResize(_pWindow);
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {
    RENDER_API::DestroyAPIWindow(_pWindow);
  }

  // Mesh

  APIMesh* CreateAPIMesh(void* _pData, size_t _uSize)
  {
    return RENDER_API::CreateAPIMesh(_pData, _uSize);
  }

  // Drawing

  int BeginDraw(APIWindow* _pWindow)
  {
    return RENDER_API::BeginDraw(_pWindow);
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount)
  {
    RENDER_API::DrawMesh(_pMesh, _uVertexCount);
  }

  void EndDraw(APIWindow* _pWindow)
  {
    RENDER_API::EndDraw(_pWindow);
  }    

}
