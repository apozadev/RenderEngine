#include "Graphics/API/DX11/RenderTargetBuilder.h"

#include "Graphics/API/DX11/APIInternal.h"

namespace api
{
namespace dx11
{

  void RenderTargetBuilder::Build(const APIWindow* pWindow)
  {     
    m_pRenderTarget->m_uMsaaSamples = m_uMsaaSamples;

    uint32_t uLayers = (m_lstColorTextures.size() > 0) ? (m_lstColorTextures[0]->m_uLayers) : 1u;

    D3D11_RENDER_TARGET_VIEW_DESC oRtvDesc = {};
    oRtvDesc.Format = m_eColorFormat;
    if (m_bIsCubemap)
    {
      oRtvDesc.ViewDimension = m_uMsaaSamples > 1u ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;      
      oRtvDesc.Texture2DArray.ArraySize = uLayers;
      oRtvDesc.Texture2DArray.FirstArraySlice = 0;      
    }
    else
    {
      oRtvDesc.ViewDimension = m_uMsaaSamples > 1u ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;      
    }    

    m_pRenderTarget->m_lstRtv.resize(m_lstColorTextures.size());
    m_pRenderTarget->m_lstResolveRtv.resize(m_lstColorResolveTextures.size());

    for (int i = 0; i < m_lstColorTextures.size(); i++)
    {
      if (m_bIsCubemap)
      {
        oRtvDesc.Texture2DArray.MipSlice = m_lstColorMipLevels[i];
      }
      else
      {
        oRtvDesc.Texture2D.MipSlice = m_lstColorMipLevels[i];
      }

      APITexture* pTexture = m_lstColorTextures[i];
      DX11_CHECK(pWindow->m_pDevice->CreateRenderTargetView(pTexture->m_pTexture.Get(), &oRtvDesc, m_pRenderTarget->m_lstRtv[i].GetAddressOf()))
    }

    for (int i = 0; i < m_lstColorResolveTextures.size(); i++)
    {
      APITexture* pTexture = m_lstColorResolveTextures[i];
      DX11_CHECK(pWindow->m_pDevice->CreateRenderTargetView(pTexture->m_pTexture.Get(), &oRtvDesc, m_pRenderTarget->m_lstRtv[i].GetAddressOf()))
    }

    if (m_pDepthStencilTexture)
    {
      D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
      descDSV.Format = DXGI_FORMAT_D32_FLOAT;//m_eDepthStencilFormat;
      if (m_bIsCubemap)
      {
        descDSV.ViewDimension = m_uMsaaSamples > 1u ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        oRtvDesc.Texture2DArray.ArraySize = uLayers;
        oRtvDesc.Texture2DArray.FirstArraySlice = 0;
        oRtvDesc.Texture2DArray.MipSlice = m_uDepthMipLevel;
      }
      else
      {
        descDSV.ViewDimension = m_uMsaaSamples > 1u ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = m_uDepthMipLevel;
      }      

      DX11_CHECK(pWindow->m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture->m_pTexture.Get(), &descDSV, m_pRenderTarget->m_pDsv.ReleaseAndGetAddressOf()));
    }

    m_pRenderTarget->m_iWidth = static_cast<int>(m_uWidth);
    m_pRenderTarget->m_iHeight = static_cast<int>(m_uHeight);
  }

  void RenderTargetBuilder::Clear()
  {
    m_lstColorTextures.clear();
    m_lstColorResolveTextures.clear();
    m_pDepthStencilTexture = nullptr;
    m_lstColorMipLevels.clear();    
    m_pRenderTarget = nullptr;
    m_bIsCubemap = false;
  }

}
}