#include "Graphics/API/Vulkan/RenderTargetBuilder.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"
#include "Graphics/API/Vulkan/APIInternal.h"
#include "Graphics/API/Vulkan/VulkanPools.h"
#include "Graphics/API/Vulkan/VulkanMacros.h"
#include "Graphics/API/Vulkan/APIWindow.h"

namespace api
{
namespace vk
{
  void RenderTargetBuilder::Build(const APIWindow* _pWindow)
  {

    bool bMultiview = false;// (m_lstColorTextures.size() > 0u) && (m_lstColorTextures[0]->m_uLayers > 1u);

    bool bHasDepthStencil = m_pDepthStencilTexture != nullptr;
    CreateRenderPass(_pWindow, m_lstColorTextures.size(), m_eColorFormat, bHasDepthStencil, m_eDepthStencilFormat, m_uMsaaSamples, m_pRenderTarget->m_hRenderPass, true, bMultiview);

    uint32_t uAttachmentCount = m_lstColorTextures.size() + m_lstColorResolveTextures.size() + (m_pDepthStencilTexture ? 1u : 0u);

    m_pRenderTarget->m_pImageViews = s_oVkImageViewPool.PullElements(uAttachmentCount);

    uint32_t uAttachmentIdx = 0u;

    for (uint32_t i = 0; i < m_lstColorTextures.size(); i++)
    {
      APITexture* pTexture = m_lstColorTextures[i];

      CreateImageView(_pWindow,
        pTexture->m_hImage,
        pTexture->m_eFormat,
        m_lstColorMipLevels[i], 1u,
        0u, pTexture->m_uLayers,
        pTexture->m_uLayers > 1u ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_COLOR_BIT,
        m_pRenderTarget->m_pImageViews[uAttachmentIdx++]);

      m_pRenderTarget->m_aColorTextures[i] = pTexture;
    }

    for (uint32_t i = 0; i < m_lstColorResolveTextures.size(); i++)
    {
      APITexture* pTexture = m_lstColorResolveTextures[i];
      CreateImageView(_pWindow,
        pTexture->m_hImage,
        pTexture->m_eFormat,
        0u, 1u,
        0u, pTexture->m_uLayers,
        pTexture->m_uLayers > 1u ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_COLOR_BIT,
        m_pRenderTarget->m_pImageViews[uAttachmentIdx++]);

      m_pRenderTarget->m_aResolveTextures[i] = pTexture;
    }    

    if(m_pDepthStencilTexture)
    {
      CreateImageView(_pWindow,
        m_pDepthStencilTexture->m_hImage,
        m_pDepthStencilTexture->m_eFormat,
        m_uDepthMipLevel, 1u,
        0u, m_pDepthStencilTexture->m_uLayers,
        m_pDepthStencilTexture->m_uLayers > 1u ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        m_pRenderTarget->m_pImageViews[uAttachmentIdx++]);

      m_pRenderTarget->m_pDepthTexture = m_pDepthStencilTexture;
    }

    VkFramebufferCreateInfo oFramebufferInfo = {};
    oFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    oFramebufferInfo.attachmentCount = uAttachmentCount;
    oFramebufferInfo.pAttachments = &m_pRenderTarget->m_pImageViews[0];
    oFramebufferInfo.renderPass = m_pRenderTarget->m_hRenderPass;
    oFramebufferInfo.width = m_uWidth;
    oFramebufferInfo.height = m_uHeight;
    oFramebufferInfo.layers = (m_lstColorTextures.size() > 0u) ? m_lstColorTextures[0]->m_uLayers : 1u;

    VK_CHECK(vkCreateFramebuffer(_pWindow->m_hDevice, &oFramebufferInfo, NULL, &m_pRenderTarget->m_hFrameBuffer))

    m_pRenderTarget->m_uWidth = m_uWidth;
    m_pRenderTarget->m_uHeight = m_uHeight;
    m_pRenderTarget->m_uMsaaSamples = m_uMsaaSamples;
  }

}
}