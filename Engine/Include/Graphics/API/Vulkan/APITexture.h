#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{  

  struct APITexture
  {
    VkImage m_hImage;
    VkDeviceMemory m_hMemory;
    VkImageView m_hImageView;
    VkSampler m_hSampler;
    VkFormat m_eFormat;
    uint32_t m_uMipLevels;
    uint32_t m_uLayers;

    APIWindow* m_pOwnerWindow;
  };

}
}
