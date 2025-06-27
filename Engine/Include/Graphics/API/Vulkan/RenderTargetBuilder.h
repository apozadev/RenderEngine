#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIRenderTarget;  

  struct APITexture;  

  struct APIWindow;

  class RenderTargetBuilder
  {
  public:

    void SetRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      m_pRenderTarget = _pRenderTarget;
    }

    void AddColorTexture(APITexture* _pTexture, uint32_t _uMipLevel)
    {
      m_lstColorTextures.push_back(_pTexture);
      m_lstColorMipLevels.push_back(_uMipLevel);
    }

    void SetDepthTexture(APITexture* _pTexture, uint32_t _uMipLevel)
    {
      m_pDepthStencilTexture = _pTexture;
      m_uDepthMipLevel = _uMipLevel;
    }

    void AddResolveColorTexture(APITexture* _pTexture)
    {
      m_lstColorResolveTextures.push_back(_pTexture);
    }

    void Build(const APIWindow* _pWindow);

    void Clear()
    {
      m_lstColorTextures.clear();
      m_lstColorResolveTextures.clear();
      m_pDepthStencilTexture = nullptr;      
    }

  public:

    VkFormat m_eColorFormat;

    VkFormat m_eDepthStencilFormat;

    uint32_t m_uMsaaSamples;

    uint32_t m_uWidth, m_uHeight;

  private:    

    APIRenderTarget* m_pRenderTarget;
    std::vector<APITexture*> m_lstColorTextures;
    std::vector<APITexture*> m_lstColorResolveTextures;
    std::vector<uint32_t> m_lstColorMipLevels;
    uint32_t m_uDepthMipLevel;
    APITexture* m_pDepthStencilTexture = nullptr;
  };

}
}