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

      void SetColorFormat(DXGI_FORMAT _eFormat)
      {
        m_eColorFormat = _eFormat;
      }

      void SetDepthStencilFormat(DXGI_FORMAT _eFormat)
      {
        m_eDepthStencilFormat = _eFormat;
      }

      void SetMsaaSamples(uint32_t _uSamples)
      {
        m_uMsaaSamples = _uSamples;
      }

      void Build(APIRenderTarget* pRenderTarget_)
      {
        APIWindow* pWindow = pRenderTarget_->m_pOwnerWindow;

        D3D11_RENDER_TARGET_VIEW_DESC oRtvDesc = {};
        oRtvDesc.Format = m_eColorFormat;
        oRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        oRtvDesc.Texture2D.MipSlice = 0;

        pRenderTarget_->m_lstRtv.resize(m_lstColorTextures.size());
        pRenderTarget_->m_lstResolveRtv.resize(m_lstColorResolveTextures.size());        

        for (int i = 0; i < m_lstColorTextures.size(); i++)
        {
          APITexture* pTexture = m_lstColorTextures[i];
          DX11_CHECK(pWindow->m_pDevice->CreateRenderTargetView(pTexture->m_pTexture.Get(), &oRtvDesc, pRenderTarget_->m_lstRtv[i].GetAddressOf()))
        }

        for (int i = 0; i < m_lstColorResolveTextures.size(); i++)
        {
          APITexture* pTexture = m_lstColorResolveTextures[i];
          DX11_CHECK(pWindow->m_pDevice->CreateRenderTargetView(pTexture->m_pTexture.Get(), &oRtvDesc, pRenderTarget_->m_lstRtv[i].GetAddressOf()))
        }

        if (m_pDepthStencilTexture)
        {
          D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
          descDSV.Format = DXGI_FORMAT_D32_FLOAT;//m_eDepthStencilFormat;
          descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
          descDSV.Texture2D.MipSlice = 0u;

          DX11_CHECK(pWindow->m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture->m_pTexture.Get(), &descDSV, pRenderTarget_->m_pDsv.ReleaseAndGetAddressOf()));
        }
      }

      void Clear()
      {
        m_lstColorTextures.clear();
        m_lstColorResolveTextures.clear();
        m_pDepthStencilTexture = nullptr;
      }

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