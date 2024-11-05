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

      void AddColorTexture(APITexture* _pTexture);

      void SetDepthTexture(APITexture* _pTexture);

      void AddResolveColorTexture(APITexture* _pTexture);

      void SetColorFormat(DXGI_FORMAT _eFormat);

      void SetDepthStencilFormat(DXGI_FORMAT _eFormat);

      void SetMsaaSamples(uint32_t _uSamples);

      void Build(APIRenderTarget* pRenderTarget_);

      void Clear();

    private:

      DXGI_FORMAT m_eColorFormat;
      DXGI_FORMAT m_eDepthStencilFormat;

      uint32_t m_uMsaaSamples;

      std::vector<APITexture*> m_lstColorTextures;
      std::vector<APITexture*> m_lstColorResolveTextures;
      APITexture* m_pDepthStencilTexture = nullptr;
    };

  }
}