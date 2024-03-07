#include "Graphics/API/GraphicsAPI.h"

#if RENDER_API == vk
#include "Graphics/API/Vulkan/VulkanAPI.h"
#endif

namespace api
{    

  void InitializeAPI()
  {
    RENDER_API::InitializeAPI();
  }

  void ShutDownAPI()
  {
    RENDER_API::ShutDownAPI();
  } 

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
  {    
    return RENDER_API::CreateAPIWindow(_pGlfwWindow);
  }

  void DrawWindow(APIWindow* _pWindow)
  {
    RENDER_API::DrawWindow(_pWindow);
  }

  void OnWindowResize(APIWindow* _pWindow)
  {
    RENDER_API::OnWindowResize(_pWindow);
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {
    RENDER_API::DestroyAPIWindow(_pWindow);
  }

}
