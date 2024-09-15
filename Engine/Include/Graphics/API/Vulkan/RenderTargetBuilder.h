#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIRenderTarget;  

  struct APITexture;  

  class RenderTargetBuilder
  {
  public:

    void AddColorTexture(APITexture* _pTexture)
    {
      m_lstColorTextures.push_back(_pTexture);
    }

    void SetDepthTexture(APITexture* _pTexture)
    {
      m_pDepthStencilTexture = _pTexture;
    }

    void AddResolveColorTexture(APITexture* _pTexture)
    {
      m_lstColorResolveTextures.push_back(_pTexture);
    }

    void SetColorFormat(VkFormat _eFormat)
    {
      m_eColorFormat = _eFormat;
    }

    void SetDepthStencilFormat(VkFormat _eFormat)
    {
      m_eDepthStencilFormat = _eFormat;
    }

    void SetMsaaSamples(uint32_t _uSamples)
    {
      m_uMsaaSamples = _uSamples;
    }

    void Build(APIRenderTarget* pRenderTarget_);

    void Clear()
    {
      m_lstColorTextures.clear();
      m_lstColorResolveTextures.clear();
      m_pDepthStencilTexture = VK_NULL_HANDLE;      
    }

  private:

    VkFormat m_eColorFormat;
    VkFormat m_eDepthStencilFormat;

    uint32_t m_uMsaaSamples;

    std::vector<APITexture*> m_lstColorTextures;
    std::vector<APITexture*> m_lstColorResolveTextures;
    APITexture* m_pDepthStencilTexture = nullptr;
  };

}
}