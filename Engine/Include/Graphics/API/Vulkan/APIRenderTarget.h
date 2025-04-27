#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIWindow;

  struct APIRenderTarget
  {
    VkRenderPass m_hRenderPass;
    VkFramebuffer m_hFrameBuffer;

    uint32_t m_uAttachmentCount;

    VkImageView* m_pImageViews;

    APIWindow* m_pOwnerWindow;

    uint32_t m_uWidth, m_uHeight;
  };

}
}