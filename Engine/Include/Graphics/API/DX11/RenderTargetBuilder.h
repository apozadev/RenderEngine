#pragma once

#include <vector>

#include <d3d11.h>

#include "DX11Macros.h"
#include "APIRenderTarget.h"
#include "APITexture.h"
#include "APIWindow.h"

namespace api
{
  namespace dx11
  {

    class RenderTargetBuilder
    {
    public:

      void AddColorTexture(APITexture* _pTexture, uint32_t _uMipLevel) { m_lstColorTextures.push_back(_pTexture); m_lstColorMipLevels.push_back(_uMipLevel); }

      void SetDepthTexture(APITexture* _pTexture, uint32_t _uMipLevel) { m_pDepthStencilTexture = _pTexture; m_uDepthMipLevel = _uMipLevel; }

      void AddResolveColorTexture(APITexture* _pTexture){ m_lstColorResolveTextures.push_back(_pTexture); }           

      void Build(const APIWindow* pWindow);

      void Clear();

    public:

      DXGI_FORMAT m_eColorFormat;
      DXGI_FORMAT m_eDepthStencilFormat;

      uint32_t m_uMsaaSamples;

      uint32_t m_uWidth, m_uHeight;

      bool m_bIsCubemap = false;

      APIRenderTarget* m_pRenderTarget = nullptr;

    private:

      std::vector<APITexture*> m_lstColorTextures;
      std::vector<APITexture*> m_lstColorResolveTextures;
      APITexture* m_pDepthStencilTexture = nullptr;      

      std::vector<uint32_t> m_lstColorMipLevels;
      uint32_t m_uDepthMipLevel;
    };

  }
}