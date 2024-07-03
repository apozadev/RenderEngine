#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Graphics/PipelineStage.h"

namespace api
{
namespace dx11
{
 
  struct APIRenderTarget
  {
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDsv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSrv;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDSTexture;

    unsigned int m_uSampleCount = 1u;
                 
    APIWindow* m_pOwnerWindow;

  };

}
}