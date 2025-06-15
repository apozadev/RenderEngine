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
    APITexture* m_pDepthStencilTexture = nullptr;
  };

}
}