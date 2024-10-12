#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace api
{
  namespace dx11
  {

    struct APIRenderTarget;

    struct APIWindow
    {
      HWND m_hWnd;

      Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
      Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
      Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapchain;  
      
      Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRtv;
      Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDsv;

      Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pColorTexture;
      Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDSTexture;      

      unsigned int m_uWidth, m_uHeight;

      unsigned int m_uNumSwapchainImages;      

      DXGI_FORMAT m_eSwapchainFormat;

      bool m_bResize = false;
    };

  }
}
