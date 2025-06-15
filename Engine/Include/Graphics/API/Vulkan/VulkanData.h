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
    VkInstance m_hInstance;    

    uint32_t m_uRenderQueueFamilyIdx;
    uint32_t m_uPresentQueueFamilyIdx;

    VkPhysicalDevice m_hPhysicalDevice;   

    VkSampleCountFlagBits m_eMaxMsaaSampleCount;

    DescriptorSetUpdater m_oDescSetUpdater;

    RenderTargetBuilder m_oRenderTargetBuilder;

    uint32_t m_uAPIMinorVersion;
    uint32_t m_uAPIMayorVersion;

    bool m_bRenderBegan = false;

  };
}
}
