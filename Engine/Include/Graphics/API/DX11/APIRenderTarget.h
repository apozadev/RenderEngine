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

    unsigned int m_uSampleCount = 1u;
                 
    APIWindow* m_pOwnerWindow;

  };

}
}