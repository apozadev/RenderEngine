#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace api
{
  namespace dx11
  {


    struct APIWindow
    {
      Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
      Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
      Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapchain;  

      unsigned int m_uWidth, m_uHeight;
    };

  }
}
