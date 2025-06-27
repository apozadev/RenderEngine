#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APITexture;

  struct APIRenderTarget
  {
    VkRenderPass m_hRenderPass;
    VkFramebuffer m_hFrameBuffer;

    uint32_t m_uAttachmentCount;

    VkImageView* m_pImageViews;

    uint32_t m_uWidth, m_uHeight;

    uint32_t m_uMsaaSamples;

    APITexture* m_aColorTextures[4];
    APITexture* m_aResolveTextures[4];
    APITexture* m_pDepthTexture;
  };

}
}