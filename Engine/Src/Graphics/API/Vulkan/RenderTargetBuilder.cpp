#include "Graphics/API/Vulkan/RenderTargetBuilder.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"
#include "Graphics/API/Vulkan/APIInternal.h"

namespace api
{
namespace vk
{
  void RenderTargetBuilder::Build(APIRenderTarget* pRenderTarget_)
  {
    CreateRenderPass(pRenderTarget_->m_pOwnerWindow, m_lstColorTextures.size(), m_eColorFormat, true, m_eDepthStencilFormat, m_uMsaaSamples, pRenderTarget_->m_hRenderPass);

    CreateFramebuffer(pRenderTarget_->m_pOwnerWindow, pRenderTarget_->m_hRenderPass, m_lstColorTextures.data(), m_lstColorTextures.size(), m_pDepthStencilTexture, m_lstColorResolveTextures.data(), pRenderTarget_->m_hFrameBuffer);
  }

}
}