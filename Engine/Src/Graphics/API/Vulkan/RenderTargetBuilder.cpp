#include "Graphics/API/Vulkan/RenderTargetBuilder.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"
#include "Graphics/API/Vulkan/APIInternal.h"
#include "Graphics/API/Vulkan/VulkanPools.h"
#include "Graphics/API/Vulkan/VulkanMacros.h"

namespace api
{
namespace vk
{
  void RenderTargetBuilder::Build(APIRenderTarget* pRenderTarget_)
  {

    bool bMultiview = false;// (m_lstColorTextures.size() > 0u) && (m_lstColorTextures[0]->m_uLayers > 1u);

    bool bHasDepthStencil = m_pDepthStencilTexture != nullptr;
    CreateRenderPass(pRenderTarget_->m_pOwnerWindow, m_lstColorTextures.size(), m_eColorFormat, bHasDepthStencil, m_eDepthStencilFormat, m_uMsaaSamples, pRenderTarget_->m_hRenderPass, true, bMultiview);

    uint32_t uAttachmentCount = m_lstColorTextures.size() + m_lstColorResolveTextures.size() + (m_pDepthStencilTexture ? 1u : 0u);

    pRenderTarget_->m_pImageViews = s_oVkImageViewPool.PullElements(uAttachmentCount);

    uint32_t uAttachmentIdx = 0u;

    for (APITexture* pTexture : m_lstColorTextures)
    {
      CreateImageView(pRenderTarget_->m_pOwnerWindow,
        pTexture->m_hImage,
        pTexture->m_eFormat,
        pTexture->m_uMipLevels,
        pTexture->m_uLayers,
        pTexture->m_uLayers > 1u ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_COLOR_BIT,
        pRenderTarget_->m_pImageViews[uAttachmentIdx++]);
    }

    for (APITexture* pTexture : m_lstColorResolveTextures)
    {
      CreateImageView(pRenderTarget_->m_pOwnerWindow,
        pTexture->m_hImage,
        pTexture->m_eFormat,
        pTexture->m_uMipLevels,
        pTexture->m_uLayers,
        pTexture->m_uLayers > 1u ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_COLOR_BIT,
        pRenderTarget_->m_pImageViews[uAttachmentIdx++]);
    }

    if(m_pDepthStencilTexture)
    {
      CreateImageView(pRenderTarget_->m_pOwnerWindow,
        m_pDepthStencilTexture->m_hImage,
        m_pDepthStencilTexture->m_eFormat,
        m_pDepthStencilTexture->m_uMipLevels,
        m_pDepthStencilTexture->m_uLayers,
        m_pDepthStencilTexture->m_uLayers > 1u ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        pRenderTarget_->m_pImageViews[uAttachmentIdx++]);
    }

    VkFramebufferCreateInfo oFramebufferInfo = {};
    oFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    oFramebufferInfo.attachmentCount = uAttachmentCount;
    oFramebufferInfo.pAttachments = &pRenderTarget_->m_pImageViews[0];
    oFramebufferInfo.renderPass = pRenderTarget_->m_hRenderPass;
    oFramebufferInfo.width = m_uWidth;
    oFramebufferInfo.height = m_uHeight;
    oFramebufferInfo.layers = (m_lstColorTextures.size() > 0u) ? m_lstColorTextures[0]->m_uLayers : 1u;

    VK_CHECK(vkCreateFramebuffer(pRenderTarget_->m_pOwnerWindow->m_hDevice, &oFramebufferInfo, NULL, &pRenderTarget_->m_hFrameBuffer))

    pRenderTarget_->m_uWidth = m_uWidth;
    pRenderTarget_->m_uHeight = m_uHeight;
  }

}
}