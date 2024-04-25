#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIRenderState;

  struct APITexture
  {
    VkImage m_hImage;
    VkDeviceMemory m_hMemory;
    VkImageView m_hImageView;
    VkSampler m_hSampler;

    APIWindow* m_pOwnerWindow;
  };

}
}
