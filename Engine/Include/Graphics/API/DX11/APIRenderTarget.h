#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <vector>

#include "Graphics/PipelineStage.h"

namespace api
{
namespace dx11
{
 
  struct APIRenderTarget
  {
    std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_lstRtv;
    std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_lstResolveRtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDsv;    

    int m_iWidth, m_iHeight;

    unsigned int m_uMsaaSamples = 1u;
                 
  };

}
}