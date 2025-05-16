#pragma once

#include <vulkan/vulkan.h>

#define TEXTURE2D_MAX_MIP_LEVELS 20

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
    uint32_t m_uWidth;
    uint32_t m_uHeight;
    VkImageLayout m_aCurrLayouts[TEXTURE2D_MAX_MIP_LEVELS];

    APIWindow* m_pOwnerWindow;
  };

}
}
