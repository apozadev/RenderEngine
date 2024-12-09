#include "Graphics/API/DX11/APIInternal.h"

#include "Core/Exception.h"
#include "Graphics/TextureUsage.h"
#include "Graphics/API/DX11/APIWindow.h"

namespace api
{
namespace dx11
{

  DX11Data s_oGlobalData;

  void PollWindowSize(APIWindow* _pWindow)
  {

    RECT rect;
    if (!GetClientRect(_pWindow->m_hWnd, &rect))
    {
      THROW_GENERIC_EXCEPTION("[DX11] Could not get window rect")
    }

    _pWindow->m_uWidth = rect.right - rect.left;
    _pWindow->m_uHeight = rect.bottom - rect.top;
  }

  DXGI_FORMAT GetDXGIFormat(ImageFormat _eFormat)
  {
    switch (_eFormat)
    {
    case ImageFormat::R8G8B8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
      break;
    case ImageFormat::R8G8B8A8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
      break;
    case ImageFormat::R32:
      return DXGI_FORMAT_R32_TYPELESS;
      break;
    default:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
  }

  DXGI_FORMAT GetDXGISrvFormat(ImageFormat _eFormat)
  {
    switch (_eFormat)
    {
    case ImageFormat::R8G8B8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
      break;
    case ImageFormat::R8G8B8A8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
      break;
    case ImageFormat::R32:
      return DXGI_FORMAT_R32_FLOAT;
      break;
    default:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
  }

  uint32_t GetTextureUsage(uint32_t _uUsage)
  {
    uint32_t uRes = 0u;

    if ((_uUsage & static_cast<int>(TextureUsage::SHADER_RESOURCE)) != 0)
    {
      uRes |= D3D11_BIND_SHADER_RESOURCE;
    }
    if ((_uUsage & static_cast<int>(TextureUsage::COLOR_TARGET)) != 0)
    {
      uRes |= D3D11_BIND_RENDER_TARGET;
    }
    if ((_uUsage & static_cast<int>(TextureUsage::DEPTH_TARGET)) != 0)
    {
      uRes |= D3D11_BIND_DEPTH_STENCIL;
    }

    return uRes;
  }

  D3D11_BLEND_OP GetD3D11BlendOp(BlendOp _eBlendOp)
  {
    switch (_eBlendOp)
    {
    case BLEND_OP_ADD:
      return D3D11_BLEND_OP_ADD;
      break;
    case BLEND_OP_SUBTRACT:
      return D3D11_BLEND_OP_SUBTRACT;
      break;
    case BLEND_OP_REV_SUBTRACT:
      return D3D11_BLEND_OP_REV_SUBTRACT;
      break;
    case BLEND_OP_MIN:
      return D3D11_BLEND_OP_REV_SUBTRACT;
      break;
    case BLEND_OP_MAX:
      return D3D11_BLEND_OP_REV_SUBTRACT;
      break;
    default:
      return D3D11_BLEND_OP_ADD;
    }
  }

  D3D11_BLEND GetD3D11Blend(BlendFactor _eBlendFactor)
  {
    switch (_eBlendFactor)
    {
    case BLEND_ZERO:
      return D3D11_BLEND_ZERO;
    case BLEND_ONE:
      return D3D11_BLEND_ONE;
    case BLEND_SRC_COLOR:
      return D3D11_BLEND_SRC_COLOR;
    case BLEND_INV_SRC_COLOR:
      return D3D11_BLEND_INV_SRC_COLOR;
    case BLEND_SRC_ALPHA:
      return D3D11_BLEND_SRC_ALPHA;
    case BLEND_INV_SRC_ALPHA:
      return D3D11_BLEND_INV_SRC_ALPHA;
    case BLEND_DEST_ALPHA:
      return D3D11_BLEND_DEST_ALPHA;
    case BLEND_INV_DEST_ALPHA:
      return D3D11_BLEND_INV_DEST_ALPHA;
    case BLEND_DEST_COLOR:
      return D3D11_BLEND_DEST_COLOR;
    case BLEND_INV_DEST_COLOR:
      return D3D11_BLEND_INV_DEST_COLOR;
    case BLEND_SRC_ALPHA_SAT:
      return D3D11_BLEND_SRC_ALPHA_SAT;
    case BLEND_BLEND_FACTOR:
      return D3D11_BLEND_BLEND_FACTOR;
    case BLEND_INV_BLEND_FACTOR:
      return D3D11_BLEND_INV_BLEND_FACTOR;
    case BLEND_SRC1_COLOR:
      return D3D11_BLEND_SRC1_COLOR;
    case BLEND_INV_SRC1_COLOR:
      return D3D11_BLEND_INV_SRC1_COLOR;
    case BLEND_SRC1_ALPHA:
      return D3D11_BLEND_SRC1_ALPHA;
    case BLEND_INV_SRC1_ALPHA:
      return D3D11_BLEND_INV_SRC1_ALPHA;
    }
  }

  ImageFormat GetImageFormat(DXGI_FORMAT _eFormat)
  {
    switch (_eFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
      return ImageFormat::R8G8B8;
      break;
    default:
      return ImageFormat::R8G8B8A8;
    }
  }

  void CreateDeviceAndSwapChain(APIWindow* _pWindow)
  {
    DXGI_SWAP_CHAIN_DESC oSwapchainDesc = {};
    oSwapchainDesc.BufferDesc.Width = _pWindow->m_uWidth;
    oSwapchainDesc.BufferDesc.Height = _pWindow->m_uHeight;
    oSwapchainDesc.BufferDesc.Format = _pWindow->m_eSwapchainFormat;
    oSwapchainDesc.BufferDesc.RefreshRate.Numerator = 0;
    oSwapchainDesc.BufferDesc.RefreshRate.Denominator = 0;
    oSwapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    oSwapchainDesc.SampleDesc.Count = 1;
    oSwapchainDesc.SampleDesc.Quality = 0;
    oSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    oSwapchainDesc.BufferCount = _pWindow->m_uNumSwapchainImages;
    oSwapchainDesc.OutputWindow = _pWindow->m_hWnd;
    oSwapchainDesc.Windowed = TRUE;
    oSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    oSwapchainDesc.Flags = 0;

    UINT uSwapCreateFlags = 0u;
#ifndef NDEBUG
    uSwapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DX11_CHECK(D3D11CreateDeviceAndSwapChain(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      uSwapCreateFlags,
      nullptr,
      0,
      D3D11_SDK_VERSION,
      &oSwapchainDesc,
      &_pWindow->m_pSwapchain,
      &_pWindow->m_pDevice,
      nullptr,
      &_pWindow->m_pContext));

  }

  void CreateSwapchainViewsAndViewport(APIWindow* _pWindow)
  {

  }

  uint32_t QueryMaxSupportedMsaaSamples(APIWindow* _pWindow)
  {
    DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // or any other render target format you're interested in
    UINT uMaxSampleCount = 0;

    for (UINT uSampleCount = 1; uSampleCount <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++uSampleCount)
    {
      UINT numQualityLevels = 0;
      HRESULT hr = _pWindow->m_pDevice->CheckMultisampleQualityLevels(eFormat, uSampleCount, &numQualityLevels);
      if (SUCCEEDED(hr) && numQualityLevels > 0)
      {
        uMaxSampleCount = uSampleCount; // Update to the highest supported sample count
      }
      else
      {
        break; // If no quality levels are supported, stop checking higher counts
      }
    }

    return uMaxSampleCount;
  }

  void CreateWindowRenderTarget(APIWindow* _pWindow, uint32_t _uMsaaSamples)
  {

    // Create Depth buffer

    D3D11_TEXTURE2D_DESC oDSTexDesc = {};
    oDSTexDesc.Width = _pWindow->m_uWidth;
    oDSTexDesc.Height = _pWindow->m_uHeight;
    oDSTexDesc.MipLevels = 1;
    oDSTexDesc.ArraySize = 1;
    oDSTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
    oDSTexDesc.SampleDesc.Count = _uMsaaSamples;
    oDSTexDesc.SampleDesc.Quality = 0;
    oDSTexDesc.Usage = D3D11_USAGE_DEFAULT;
    oDSTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    oDSTexDesc.CPUAccessFlags = 0;
    oDSTexDesc.MiscFlags = 0;
    DX11_CHECK(_pWindow->m_pDevice->CreateTexture2D(&oDSTexDesc, NULL, _pWindow->m_pDSTexture.GetAddressOf()));

    // Create DSV      

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2D.MipSlice = 0u;
    DX11_CHECK(_pWindow->m_pDevice->CreateDepthStencilView(_pWindow->m_pDSTexture.Get(), &descDSV, _pWindow->m_pDsv.GetAddressOf()));

    // Create color texture

    D3D11_TEXTURE2D_DESC oTexDesc = {};
    oTexDesc.Width = _pWindow->m_uWidth;
    oTexDesc.Height = _pWindow->m_uHeight;
    oTexDesc.Format = _pWindow->m_eSwapchainFormat;
    oTexDesc.MipLevels = 1;
    oTexDesc.ArraySize = 1;
    oTexDesc.SampleDesc.Count = _uMsaaSamples;
    oTexDesc.SampleDesc.Quality = 0;
    oTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    oTexDesc.Usage = D3D11_USAGE_DEFAULT;
    oTexDesc.CPUAccessFlags = 0;

    DX11_CHECK(_pWindow->m_pDevice->CreateTexture2D(&oTexDesc, NULL, _pWindow->m_pColorTexture.GetAddressOf()));

    // Create Render Target View

    D3D11_RENDER_TARGET_VIEW_DESC oRtvDesc = {};
    oRtvDesc.Format = _pWindow->m_eSwapchainFormat;
    oRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    oRtvDesc.Texture2D.MipSlice = 0;

    DX11_CHECK(_pWindow->m_pDevice->CreateRenderTargetView(_pWindow->m_pColorTexture.Get(), &oRtvDesc, _pWindow->m_pRtv.GetAddressOf()))

  }

  void ResizeSwapchain(APIWindow* _pWindow)
  {
    PollWindowSize(_pWindow);

    _pWindow->m_pSwapchain->ResizeBuffers(_pWindow->m_uNumSwapchainImages, _pWindow->m_uWidth, _pWindow->m_uHeight, _pWindow->m_eSwapchainFormat, 0u);

    CreateSwapchainViewsAndViewport(_pWindow);
  }

}
}
