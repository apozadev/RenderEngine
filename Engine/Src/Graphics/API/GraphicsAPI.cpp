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
}
