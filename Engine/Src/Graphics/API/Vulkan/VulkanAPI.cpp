#include "Graphics/API/Vulkan/VulkanAPI.h"

#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "GLFW\glfw3.h"

#include "Graphics/API/Vulkan/VulkanMacros.h"
#include "Graphics/API/Vulkan/VulkanData.h"

namespace api
{
namespace vk
{

  VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT       /*flags*/,
    VkDebugReportObjectTypeEXT  /*objectType*/,
    uint64_t                    /*object*/,
    size_t                      /*location*/,
    int32_t                     /*messageCode*/,
    const char*                 /*pLayerPrefix*/,
    const char* pMessage,
    void*                       /*pUserData*/)
  {
    OutputDebugStringA(pMessage);
    OutputDebugStringA("\n");
    return VK_FALSE;
  }

  VulkanData s_oGlobalData;

  constexpr uint32_t QUEUE_COUNT = 1u;
  constexpr uint32_t CMD_BUFFER_COUNT = 3u;

  void InitializeAPI()
  {

    // Create vulkan instance

#ifdef _DEBUG
    const char* pDebugLayers[] = { "VK_LAYER_KHRONOS_validation" };
    uint32_t uLayerCount = 1u;
#else
    const char* pDebugLayers[] = {};
    uint32_t uLayerCount = 0u;
#endif

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo oInstanceCreateInfo = {};
    oInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    oInstanceCreateInfo.pNext = NULL;
    oInstanceCreateInfo.flags = 0u;   
    oInstanceCreateInfo.enabledLayerCount = uLayerCount;
    oInstanceCreateInfo.ppEnabledLayerNames = pDebugLayers;
    oInstanceCreateInfo.enabledExtensionCount = 1u;// glfwExtensionCount;
    oInstanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    VkApplicationInfo oApplicationInfo = {};
    oApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    oApplicationInfo.apiVersion = VK_API_VERSION_1_0;
    oApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    oApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    oApplicationInfo.pApplicationName = "App";
    oApplicationInfo.pEngineName = "RenderEngine";

    oInstanceCreateInfo.pApplicationInfo = &oApplicationInfo;

    s_oGlobalData.m_hInstance = VK_NULL_HANDLE;
    VK_CHECK(vkCreateInstance(&oInstanceCreateInfo, nullptr, &s_oGlobalData.m_hInstance))        

    // Find physical device with graphics capabilities

      uint32_t uPhysicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(s_oGlobalData.m_hInstance, &uPhysicalDeviceCount, nullptr))
      VkPhysicalDevice* pPhysicalDevices = new VkPhysicalDevice[uPhysicalDeviceCount];
    VK_CHECK(vkEnumeratePhysicalDevices(s_oGlobalData.m_hInstance, &uPhysicalDeviceCount, pPhysicalDevices))
      VkPhysicalDevice hSelectedPhysicalDevice = VK_NULL_HANDLE;
    int uQueueFamilyIdx = 0u;
    for (int i = 0; i < uPhysicalDeviceCount; i++)
    {
      VkPhysicalDevice hPhysicalDevice = pPhysicalDevices[i];
      uint32_t uQueueFamilyPropertyCount = 0u;
      vkGetPhysicalDeviceQueueFamilyProperties(hPhysicalDevice,
        &uQueueFamilyPropertyCount, nullptr);
      VkQueueFamilyProperties* pQueueFamilyProperties = new VkQueueFamilyProperties[uQueueFamilyPropertyCount];
      vkGetPhysicalDeviceQueueFamilyProperties(hPhysicalDevice,
        &uQueueFamilyPropertyCount, pQueueFamilyProperties);
      for (uint32_t j = 0; j < uQueueFamilyPropertyCount; j++)
      {
        const VkQueueFamilyProperties& rQueueFamilyProperty = pQueueFamilyProperties[j];
        if (rQueueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
          hSelectedPhysicalDevice = hPhysicalDevice;
          uQueueFamilyIdx = j;
        }
        ++i;
      }
    }

    if (hSelectedPhysicalDevice == VK_NULL_HANDLE)
    {
      THROW_GENERIC_EXCEPTION("No device with graphics capabilities was found")
    }

    // Create logical device

    VkDeviceQueueCreateInfo oDeviceQueueCreateInfo = {};
    oDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    oDeviceQueueCreateInfo.queueCount = QUEUE_COUNT;
    oDeviceQueueCreateInfo.queueFamilyIndex = uQueueFamilyIdx;
    static const float queuePriorities[] = { 1.0f };
    oDeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo oDeviceCreateInfo = {};
    oDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    oDeviceCreateInfo.queueCreateInfoCount = 1u;
    oDeviceCreateInfo.pQueueCreateInfos = &oDeviceQueueCreateInfo;
    oDeviceCreateInfo.ppEnabledLayerNames = NULL;
    oDeviceCreateInfo.enabledLayerCount = 0u;
    
    VK_CHECK(vkCreateDevice(hSelectedPhysicalDevice, &oDeviceCreateInfo, nullptr, &s_oGlobalData.m_hDevice))

      VkQueue hQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(s_oGlobalData.m_hDevice, uQueueFamilyIdx, 0, &hQueue);

    if (hQueue == VK_NULL_HANDLE)
    {
      THROW_GENERIC_EXCEPTION("WTF Vulkan??")
    }

    // Set up debug output callback

    s_oGlobalData.m_oImportTable = VulkanData::ImportTable(s_oGlobalData.m_hInstance, s_oGlobalData.m_hDevice);

#ifdef _DEBUG    

    s_oGlobalData.m_hDebugCallback = VK_NULL_HANDLE;

    if (s_oGlobalData.m_oImportTable.vkCreateDebugReportCallbackEXT)
    {
      VkDebugReportCallbackCreateInfoEXT oCallbackCreateInfo = {};
      oCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
      oCallbackCreateInfo.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
      oCallbackCreateInfo.pfnCallback = &DebugReportCallback;

      VK_CHECK(s_oGlobalData.m_oImportTable.vkCreateDebugReportCallbackEXT(s_oGlobalData.m_hInstance, &oCallbackCreateInfo, nullptr, &s_oGlobalData.m_hDebugCallback))
    }
#endif

    // Create command pool

    VkCommandPoolCreateInfo oCmdPoolCreateInfo = {};
    oCmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    oCmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    oCmdPoolCreateInfo.queueFamilyIndex = uQueueFamilyIdx;
    oCmdPoolCreateInfo.pNext = NULL;
    VK_CHECK(vkCreateCommandPool(s_oGlobalData.m_hDevice, &oCmdPoolCreateInfo, NULL, &s_oGlobalData.m_hCmdPool))

    // Create command buffer

    s_oGlobalData.m_uCmdBufferCount = CMD_BUFFER_COUNT;
    s_oGlobalData.m_pCmdBuffers = new VkCommandBuffer[s_oGlobalData.m_uCmdBufferCount];
    VkCommandBufferAllocateInfo oCmdBufferAllocInfo = {};
    oCmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    oCmdBufferAllocInfo.commandPool = s_oGlobalData.m_hCmdPool;
    oCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    oCmdBufferAllocInfo.commandBufferCount = s_oGlobalData.m_uCmdBufferCount;
    VK_CHECK(vkAllocateCommandBuffers(s_oGlobalData.m_hDevice, &oCmdBufferAllocInfo, s_oGlobalData.m_pCmdBuffers))

    // Create fences for command buffers

    s_oGlobalData.m_uFenceCount = s_oGlobalData.m_uCmdBufferCount - 1u;
    s_oGlobalData.m_pFences = new VkFence[s_oGlobalData.m_uFenceCount];
    for (int i = 0; i < s_oGlobalData.m_uFenceCount; ++i)
    {
      VkFenceCreateInfo fenceCreateInfo = {};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      // We need this so we can wait for them on the first try
      fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
      VK_CHECK(vkCreateFence(s_oGlobalData.m_hDevice, &fenceCreateInfo, NULL, &s_oGlobalData.m_pFences[i]))
    }

  }

  void ShutDownAPI()
  {
    for (int i = 0; i < s_oGlobalData.m_uFenceCount; ++i)
    {
      vkDestroyFence(s_oGlobalData.m_hDevice, s_oGlobalData.m_pFences[i], nullptr);
    }

    //vkDestroyRenderPass(s_oGlobalData.m_hDevice, renderPass_, nullptr);

    /*for (int i = 0; i < QUEUE_SLOT_COUNT; ++i)
    {
      vkDestroyFramebuffer(s_oGlobalData.m_hDevice, framebuffer_[i], nullptr);
      vkDestroyImageView(s_oGlobalData.m_hDevice, swapChainImageViews_[i], nullptr);
    }*/

    vkDestroyCommandPool(s_oGlobalData.m_hDevice, s_oGlobalData.m_hCmdPool, nullptr);

    //vkDestroySwapchainKHR(s_oGlobalData.m_hDevice, swapchain_, nullptr);
    //vkDestroySurfaceKHR(instance_, surface_, nullptr);

#ifdef _DEBUG
    if(s_oGlobalData.m_hDebugCallback)
    {
    s_oGlobalData.m_oImportTable.vkDestroyDebugReportCallbackEXT(s_oGlobalData.m_hInstance, s_oGlobalData.m_hDebugCallback, nullptr);
    }
#endif

    vkDestroyDevice(s_oGlobalData.m_hDevice, nullptr);
    vkDestroyInstance(s_oGlobalData.m_hInstance, nullptr);
  }

}
}
