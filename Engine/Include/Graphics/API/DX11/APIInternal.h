#pragma once

#include <d3d11.h>
#include <inttypes.h>

#include "Graphics/ImageFormat.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/API/DX11/DX11Data.h"

namespace api
{
namespace dx11
{

  extern DX11Data s_oGlobalData;

  void PollWindowSize(APIWindow* _pWindow);

  DXGI_FORMAT GetDXGIFormat(ImageFormat _eFormat);

  DXGI_FORMAT GetDXGISrvFormat(ImageFormat _eFormat);

  uint32_t GetTextureUsage(uint32_t _uUsage);

  D3D11_BLEND_OP GetD3D11BlendOp(BlendOp _eBlendOp);

  D3D11_BLEND GetD3D11Blend(BlendFactor _eBlendFactor);

  ImageFormat GetImageFormat(DXGI_FORMAT _eFormat);

  void CreateDeviceAndSwapChain(APIWindow* _pWindow);

  void CreateSwapchainViewsAndViewport(APIWindow* _pWindow);

  uint32_t QueryMaxSupportedMsaaSamples(APIWindow* _pWindow);

  void CreateWindowRenderTarget(APIWindow* _pWindow, uint32_t _uMsaaSamples);

  void ResizeSwapchain(APIWindow* _pWindow);

}
}
