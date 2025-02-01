#include "Graphics/API/DX11/RenderTargetBuilder.h"

#include "Graphics/API/DX11/APIInternal.h"

namespace api
{
namespace dx11
{

  void RenderTargetBuilder::Build(APIRenderTarget* pRenderTarget_)
  {
    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

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

    pRenderTarget_->m_iWidth = static_cast<int>(m_uWidth);
    pRenderTarget_->m_iHeight = static_cast<int>(m_uHeight);
  }

  void RenderTargetBuilder::Clear()
  {
    m_lstColorTextures.clear();
    m_lstColorResolveTextures.clear();
    m_pDepthStencilTexture = nullptr;
  }

}
}