#pragma once

#include <d3d11.h>
#include <inttypes.h>

#include "Graphics/ImageFormat.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/CullMode.h"
#include "Graphics/DepthCompareOp.h"
#include "Graphics/SamplerConfig.h"
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

  D3D11_TEXTURE_ADDRESS_MODE GetD3D11AdressMode(TextureAddressMode _eAddressMode);

  D3D11_FILTER GetD3D11Filter(TextureFilterMode _eMipmapFilter, TextureFilterMode _eMinFilter, TextureFilterMode _eMagFilter);

  D3D11_CULL_MODE GetD3D11CullMode(CullMode _eCullMode);

  D3D11_COMPARISON_FUNC GetD3D11DepthCompareFunc(DepthCompareOp _eCompareOp);

  void CreateDeviceAndSwapChain(APIWindow* _pWindow);

  void CreateSwapchainViewsAndViewport(APIWindow* _pWindow);

  uint32_t QueryMaxSupportedMsaaSamples(APIWindow* _pWindow);

  void CreateWindowRenderTarget(APIWindow* _pWindow, uint32_t _uMsaaSamples);

  void ResizeSwapchain(APIWindow* _pWindow);

}
}
