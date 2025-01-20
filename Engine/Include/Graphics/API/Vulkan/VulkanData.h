#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/API/Vulkan/DescriptorUtils.h"
#include "Graphics/API/Vulkan/RenderTargetBuilder.h"

namespace api
{
namespace vk
{

  struct APIWindow;
  struct APIRenderState;
  struct APIRenderSubState;
  struct APIRenderTarget;  

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

    uint32_t m_uRenderQueueFamilyIdx;
    uint32_t m_uPresentQueueFamilyIdx;

    VkPhysicalDevice m_hPhysicalDevice;   

    VkSampleCountFlagBits m_eMaxMsaaSampleCount;

    APIWindow* m_pUsingWindow = nullptr;

    APIRenderState* m_pUsingRenderState = nullptr;
    APIRenderSubState* m_pUsingSubState = nullptr;
    APIRenderTarget* m_pUsingRenderTarget = nullptr;

    APIRenderTarget* m_pBoundRenderTarget = nullptr;    

    DescriptorSetUpdater m_oDescSetUpdater;

    RenderTargetBuilder m_oRenderTargetBuilder;

    uint32_t m_uAPIMinorVersion;
    uint32_t m_uAPIMayorVersion;

    bool m_bRenderBegan = false;

  };
}
}
