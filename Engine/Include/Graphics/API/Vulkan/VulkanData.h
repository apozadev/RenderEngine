#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{
  struct VulkanData
  {

    struct ImportTable
    {
#define GET_INSTANCE_ENTRYPOINT(i, w) w = reinterpret_cast<PFN_##w>(vkGetInstanceProcAddr(i, #w))
#define GET_DEVICE_ENTRYPOINT(i, w) w = reinterpret_cast<PFN_##w>(vkGetDeviceProcAddr(i, #w))

      ImportTable() = default;

      ImportTable(VkInstance instance, VkDevice device)
      {
#ifdef _DEBUG
        GET_INSTANCE_ENTRYPOINT(instance, vkCreateDebugReportCallbackEXT);
        GET_INSTANCE_ENTRYPOINT(instance, vkDebugReportMessageEXT);
        GET_INSTANCE_ENTRYPOINT(instance, vkDestroyDebugReportCallbackEXT);
#endif
      }

#undef GET_INSTANCE_ENTRYPOINT
#undef GET_DEVICE_ENTRYPOINT

#ifdef _DEBUG
      PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
      PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = nullptr;
      PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
#endif
    };

#ifdef _DEBUG    
    VkDebugReportCallbackEXT m_hDebugCallback;
#endif

    ImportTable m_oImportTable;

    VkInstance m_hInstance;

    VkDevice m_hDevice;

    VkCommandPool m_hCmdPool;

    VkCommandBuffer* m_pCmdBuffers;
    uint32_t m_uCmdBufferCount;

    VkFence* m_pFences;
    uint32_t m_uFenceCount;
  };
}
}
