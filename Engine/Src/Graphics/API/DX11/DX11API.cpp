#pragma once

#include "Graphics/API/DX11/DX11API.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <dxgi.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Core/Exception.h"
#include "File/FileUtils.h"
#include "File/File.h"
#include "Util/WideStringUtil.h"
#include "Graphics/Mesh.h"
#include "Graphics/Vertex.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/API/DX11/DX11Macros.h"
#include "Graphics/API/DX11/DX11Data.h"
#include "Graphics/API/DX11/APIWindow.h"
#include "Graphics/API/DX11/APICamera.h"
#include "Graphics/API/DX11/APIRenderState.h"
#include "Graphics/API/DX11/APIConstantBuffer.h"
#include "Graphics/API/DX11/APIMesh.h"
#include "Graphics/API/DX11/APITexture.h"
#include "Graphics/API/DX11/APIRenderTarget.h"
#include "Graphics/API/DX11/DX11BindSlotOffset.h"

namespace wrl = Microsoft::WRL;

namespace api
{
  namespace dx11
  {

    DX11Data s_oGlobalData;

    void PollWindowSize(APIWindow* _pWindow)
    {      

      RECT rect;
      if (!GetWindowRect(_pWindow->m_hWnd, &rect))
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
      default:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
      }
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
      
      // Get back buffer      
      ID3D11Texture2D* pBackBuffer = nullptr;
      DX11_CHECK(_pWindow->m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
      DX11_CHECK(_pWindow->m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, _pWindow->m_pBackBufferRTV.GetAddressOf()));

      /*
      D3D11_TEXTURE2D_DESC oRtDesc;
      pBackBuffer->GetDesc(&oRtDesc);

      // Create Depth buffer
      wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
      D3D11_TEXTURE2D_DESC oTexDesc;
      oTexDesc.Width = oRtDesc.Width;
      oTexDesc.Height = oRtDesc.Height;
      oTexDesc.MipLevels = 1;
      oTexDesc.ArraySize = 1;
      oTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
      oTexDesc.SampleDesc.Count = 1;
      oTexDesc.SampleDesc.Quality = 0;
      oTexDesc.Usage = D3D11_USAGE_DEFAULT;
      oTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      oTexDesc.CPUAccessFlags = 0;
      oTexDesc.MiscFlags = 0;
      DX11_CHECK(_pWindow->m_pDevice->CreateTexture2D(&oTexDesc, NULL, pDepthStencil.GetAddressOf()));

      D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
      descDSV.Format = DXGI_FORMAT_D32_FLOAT;
      descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      descDSV.Texture2D.MipSlice = 0u;
      DX11_CHECK(_pWindow->m_pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, _pWindow->m_pDepthStencilView.GetAddressOf()));
      */
    }

    void ResizeSwapchain(APIWindow* _pWindow)
    {
      PollWindowSize(_pWindow);

      _pWindow->m_pBackBufferRTV.ReleaseAndGetAddressOf();
      //_pWindow->m_pDepthStencilView.ReleaseAndGetAddressOf();

      _pWindow->m_pSwapchain->ResizeBuffers(_pWindow->m_uNumSwapchainImages, _pWindow->m_uWidth, _pWindow->m_uHeight, _pWindow->m_eSwapchainFormat, 0u);

      CreateSwapchainViewsAndViewport(_pWindow);
    }    

    // General

    void InitializeAPI()
    {

    }

    void ShutDownAPI()
    {

    }

    // Window

    APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
    {

      APIWindow* pWindow = new APIWindow();  

      pWindow->m_hWnd = glfwGetWin32Window(_pGlfwWindow);

      pWindow->m_uNumSwapchainImages = 2u;

      pWindow->m_eSwapchainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
      
      PollWindowSize(pWindow);

      CreateDeviceAndSwapChain(pWindow);

      CreateSwapchainViewsAndViewport(pWindow);

      s_oGlobalData.m_pUsingWindow = pWindow;

      pWindow->m_pMsaaRenderTarget = CreateAPIRenderTarget(pWindow->m_uWidth, pWindow->m_uHeight, GetImageFormat(pWindow->m_eSwapchainFormat), true);

      // Default to triangle list just in case
      pWindow->m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      return pWindow;
    }

    void SetUsingAPIWindow(APIWindow* _pWindow)
    {
      s_oGlobalData.m_pUsingWindow = _pWindow;
    }

    void OnWindowResize(APIWindow* _pWindow)
    {

      _pWindow->m_bResize = true;

    }

    uint32_t GetWindowWidth(APIWindow* _pWindow)
    {
      return _pWindow->m_uWidth;
    }

    uint32_t GetWindowHeight(APIWindow* _pWindow)
    {
      return _pWindow->m_uHeight;
    }

    void DestroyAPIWindow(APIWindow* _pAPIWindow)
    {
      DestroyAPIRenderTarget(_pAPIWindow->m_pMsaaRenderTarget);
      delete _pAPIWindow;
    }

    // Camera

    APICamera* CreateAPICamera()
    {
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;
      APICamera* pCamera = new APICamera();
      pCamera->m_pOwnerWindow = pWindow;      

      pCamera->m_oViewport = {};
      pCamera->m_oViewport.TopLeftX = 0.f;
      pCamera->m_oViewport.TopLeftY = 0.f;
      pCamera->m_oViewport.Width = static_cast<float>(pWindow->m_uWidth);
      pCamera->m_oViewport.Height = static_cast<float>(pWindow->m_uHeight);
      pCamera->m_oViewport.MaxDepth = 1;
      pCamera->m_oViewport.MinDepth = 0;

      return pCamera;
    }

    void BeginCameraSubStateSetup(APICamera* _pCamera)
    {      
    }

    void EndCameraSubstateSetup(APICamera* _pCamera)
    {
    }

    void BindAPICamera(APICamera* _pCamera)
    {
      APIWindow* pWindow = _pCamera->m_pOwnerWindow;

      pWindow->m_pContext->RSSetViewports(1u, &_pCamera->m_oViewport);
    }

    void DestroyAPICamera(APICamera* _pCamera)
    {
      delete _pCamera;
    }

    // Mesh

    APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize)
    {

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      APIMesh* pMesh = new APIMesh();

      pMesh->m_pOwnerWindow = pWindow;

      // Vertex buffer

      D3D11_BUFFER_DESC oVBDesc = {};
      oVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      oVBDesc.Usage = D3D11_USAGE_IMMUTABLE;
      oVBDesc.CPUAccessFlags = 0u;
      oVBDesc.MiscFlags = 0u;
      oVBDesc.ByteWidth = _uVertexDataSize;
      oVBDesc.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA oVBData = {};
      oVBData.pSysMem = _pVertexData;

      DX11_CHECK(pWindow->m_pDevice->CreateBuffer(&oVBDesc, &oVBData, pMesh->m_pVertexBuffer.GetAddressOf()))

      // Index buffer

      D3D11_BUFFER_DESC oIBDesc = {};
      oIBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      oIBDesc.Usage = D3D11_USAGE_IMMUTABLE;
      oIBDesc.CPUAccessFlags = 0u;
      oIBDesc.MiscFlags = 0u;
      oIBDesc.ByteWidth = _uIndexDataSize;
      oIBDesc.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA oIBData = {};
      oIBData.pSysMem = _pIndexData;
      oIBData.SysMemPitch = 0;
      oIBData.SysMemSlicePitch = 0;

      DX11_CHECK(pWindow->m_pDevice->CreateBuffer(&oIBDesc, &oIBData, pMesh->m_pIndexBuffer.GetAddressOf()))

      pMesh->m_uIndexCount = _uIndexDataSize / sizeof(uint16_t);

      // Model CBuffer
      pMesh->m_pModelCBuffer = CreateAPIConstantBuffer(sizeof(MeshConstant));

      ResourceBindInfo oBindInfo = {};
      oBindInfo.m_eLevel = ResourceFrequency::MATERIAL_INSTANCE;
      oBindInfo.m_eStage = PipelineStage::VERTEX;
      oBindInfo.m_iBinding = 0u;

      SubStateSetupConstantBuffer(pMesh->m_pModelCBuffer, sizeof(MeshConstant), oBindInfo);

      return pMesh;
    }

    void DestroyAPIMesh(APIMesh* _pMesh)
    {
      DestroyAPIConstanBuffer(_pMesh->m_pModelCBuffer);
      delete _pMesh;
    }

    // Constant buffer

    APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
    {
      APIConstantBuffer* pCBuffer = new APIConstantBuffer();

      pCBuffer->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;
      
      return pCBuffer;
    }

    void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize)
    {            
      APIWindow* pWindow = _pCBuffer->m_pOwnerWindow;

      /*D3D11_MAPPED_SUBRESOURCE oMsr;
      DX11_CHECK(pWindow->m_pContext->Map(_pCBuffer->m_pCBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &oMsr));

      memcpy(oMsr.pData, &_pData, _uSize);

      pWindow->m_pContext->Unmap(_pCBuffer->m_pCBuffer.Get(), 0u);*/

      pWindow->m_pContext->UpdateSubresource(_pCBuffer->m_pCBuffer.Get(), 0u, nullptr, _pData, 0u, 0u);

    }

    void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
    {

      APIWindow* pWindow = _pCbuffer->m_pOwnerWindow;

      uint32_t uStageMsk = _pCbuffer->m_stageMask;

      if ((uStageMsk & static_cast<uint32_t>(PipelineStage::VERTEX)) != 0u)
      {
        pWindow->m_pContext->VSSetConstantBuffers(_pCbuffer->m_slot, 1u, _pCbuffer->m_pCBuffer.GetAddressOf());
      }
      if ((uStageMsk & static_cast<uint32_t>(PipelineStage::PIXEL)) != 0u)
      {
        pWindow->m_pContext->PSSetConstantBuffers(_pCbuffer->m_slot, 1u, _pCbuffer->m_pCBuffer.GetAddressOf());
      }

    }

    void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer)
    {
      delete _pCBuffer;
    }

    // Texture

    APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels)
    {
      APITexture* pTexture = new APITexture();

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      pTexture->m_pOwnerWindow = pWindow;

      uint32_t uPixelSize = GetImageFormatSize(_eFormat);

      uint32_t uMemPitch = uPixelSize * _uWidth;

      DXGI_FORMAT eDXGIFormat = GetDXGIFormat(_eFormat);

      uint32_t uMaxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_uWidth, _uWidth)))) + 1;
      _uMipLevels = _uMipLevels == 0u ? uMaxMipLevels : std::min(_uMipLevels, uMaxMipLevels);

      D3D11_SUBRESOURCE_DATA oData = {};
      oData.pSysMem = _pData;
      oData.SysMemPitch = uMemPitch;

      D3D11_TEXTURE2D_DESC oTexDesc = {};
      oTexDesc.ArraySize = 1;
      oTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
      oTexDesc.CPUAccessFlags = 0;
      oTexDesc.Format = eDXGIFormat;
      oTexDesc.SampleDesc.Count = 1;
      oTexDesc.SampleDesc.Quality = 0;
      oTexDesc.Usage = D3D11_USAGE_DEFAULT;
      oTexDesc.MipLevels = _uMipLevels;
      oTexDesc.Width = _uWidth;
      oTexDesc.Height = _uHeight;

      if (_uMipLevels != 1)
      {
        oTexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
      }

      if (_uMipLevels == 1)
      {
        DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&oTexDesc, &oData, pTexture->m_pTexture.GetAddressOf()));
      }
      else
      {
        //GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf()));
        DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&oTexDesc, nullptr, pTexture->m_pTexture.GetAddressOf()));
      }

      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
      srvDesc.Format = eDXGIFormat;
      srvDesc.Texture2D.MipLevels = _uMipLevels;
      srvDesc.Texture2D.MostDetailedMip = 0u;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      DX11_CHECK(pWindow->m_pDevice->CreateShaderResourceView(pTexture->m_pTexture.Get(), &srvDesc, pTexture->m_pSRV.GetAddressOf()));

      if (_uMipLevels != 1)
      {
        pWindow->m_pContext->UpdateSubresource(pTexture->m_pTexture.Get(), 0u, nullptr, _pData, uMemPitch, 0u);
        pWindow->m_pContext->GenerateMips(pTexture->m_pSRV.Get());
      }

      pTexture->m_eFormat = eDXGIFormat;
      pTexture->m_iWidth = _uWidth;
      pTexture->m_iHeight = _uHeight;

      return pTexture;
    }

    void BindAPITexture(APITexture* _pTexture)
    {
      APIWindow* pWindow = _pTexture->m_pOwnerWindow;

      uint32_t uStageMsk = static_cast<uint32_t>(_pTexture->m_eStage);

      if ((uStageMsk & static_cast<uint32_t>(PipelineStage::VERTEX)) != 0u)
      {
        pWindow->m_pContext->VSSetShaderResources(_pTexture->m_uSlot, 1u, _pTexture->m_pSRV.GetAddressOf());
      }
      if ((uStageMsk & static_cast<uint32_t>(PipelineStage::PIXEL)) != 0u)
      {
        pWindow->m_pContext->PSSetShaderResources(_pTexture->m_uSlot, 1u, _pTexture->m_pSRV.GetAddressOf());
      }
    }

    void DestroyAPITexture(APITexture* _pTexture)
    {
      delete _pTexture;
    }

    // RenderTarget

    APIRenderTarget* CreateAPIRenderTarget(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, bool _bMultisampled)
    {

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      APIRenderTarget* pRenderTarget = new APIRenderTarget();

      pRenderTarget->m_pOwnerWindow = pWindow;

      uint32_t uSampleCount = _bMultisampled ? 8u : 1u;

      // Create Depth buffer
      D3D11_TEXTURE2D_DESC oDSTexDesc = {};
      oDSTexDesc.Width = pWindow->m_uWidth;
      oDSTexDesc.Height = pWindow->m_uHeight;
      oDSTexDesc.MipLevels = 1;
      oDSTexDesc.ArraySize = 1;
      oDSTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
      oDSTexDesc.SampleDesc.Count = uSampleCount;
      oDSTexDesc.SampleDesc.Quality = 0;
      oDSTexDesc.Usage = D3D11_USAGE_DEFAULT;
      oDSTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      oDSTexDesc.CPUAccessFlags = 0;
      oDSTexDesc.MiscFlags = 0;      
      DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&oDSTexDesc, NULL, pRenderTarget->m_pDSTexture.GetAddressOf()));

      D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
      descDSV.Format = DXGI_FORMAT_D32_FLOAT;
      descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
      descDSV.Texture2D.MipSlice = 0u;
      DX11_CHECK(pWindow->m_pDevice->CreateDepthStencilView(pRenderTarget->m_pDSTexture.Get(), &descDSV, pRenderTarget->m_pDsv.GetAddressOf()));

      // Create color texture

      DXGI_FORMAT eDXGIFormat = GetDXGIFormat(_eFormat);

      D3D11_TEXTURE2D_DESC oTexDesc = {};
      oTexDesc.Width = pWindow->m_uWidth;
      oTexDesc.Height = pWindow->m_uHeight;
      oTexDesc.Format = eDXGIFormat;
      oTexDesc.MipLevels = 1;
      oTexDesc.ArraySize = 1;
      oTexDesc.SampleDesc.Count = uSampleCount;
      oTexDesc.SampleDesc.Quality = 0;
      oTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
      oTexDesc.Usage = D3D11_USAGE_DEFAULT;
      oTexDesc.CPUAccessFlags = 0;

      DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&oTexDesc, NULL, pRenderTarget->m_pTexture.GetAddressOf()));

      // Create Shader Resource View

      D3D11_SHADER_RESOURCE_VIEW_DESC oSrvDesc = {};
      oSrvDesc.Format = eDXGIFormat;
      oSrvDesc.Texture2D.MipLevels = 1;
      oSrvDesc.Texture2D.MostDetailedMip = 0u;
      oSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
      DX11_CHECK(pWindow->m_pDevice->CreateShaderResourceView(pRenderTarget->m_pTexture.Get(), &oSrvDesc, pRenderTarget->m_pSrv.GetAddressOf()));

      // Create Render Target View

      D3D11_RENDER_TARGET_VIEW_DESC oRtvDesc = {};
      oRtvDesc.Format = eDXGIFormat;
      oRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
      oRtvDesc.Texture2D.MipSlice = 0;

      DX11_CHECK(pWindow->m_pDevice->CreateRenderTargetView(pRenderTarget->m_pTexture.Get(), &oRtvDesc, pRenderTarget->m_pRtv.GetAddressOf()))

      return pRenderTarget;

    }

    void BindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      APIWindow* pWindow = _pRenderTarget->m_pOwnerWindow;

      float aClearColor[] = { 0.f,0.f,0.f,1.f };
      unsigned int uClearFlags = D3D11_CLEAR_DEPTH;

      pWindow->m_pContext->ClearRenderTargetView(_pRenderTarget->m_pRtv.Get(), aClearColor);
      pWindow->m_pContext->ClearDepthStencilView(_pRenderTarget->m_pDsv.Get(), uClearFlags, 1.0f, 0u);

      pWindow->m_pContext->OMSetRenderTargets(1u, _pRenderTarget->m_pRtv.GetAddressOf(), _pRenderTarget->m_pDsv.Get());        
    }

    void CopyRenderTarget(APIRenderTarget* _pSrc, APIRenderTarget* _pDst)
    {

      if (_pSrc->m_pOwnerWindow != _pDst->m_pOwnerWindow)
      {
        THROW_GENERIC_EXCEPTION("Trying to copy a rendertarget to another from a different window!")
      }      

      APIWindow* pWindow = _pSrc->m_pOwnerWindow;

      pWindow->m_pContext->CopyResource(_pDst->m_pTexture.Get(), _pSrc->m_pTexture.Get());
      
    }

    void DestroyAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      delete _pRenderTarget;
    }

    // Render state

    APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo)
    {

      APIRenderState* pRenderState = new APIRenderState();

      pRenderState->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

      APIWindow* pWindow = pRenderState->m_pOwnerWindow;

      std::string sPSFilename = _oInfo.m_sPSFilename.substr(0, _oInfo.m_sPSFilename.find_last_of('.')) + ".cso";
      std::string sVSFilename = _oInfo.m_sVSFilename.substr(0, _oInfo.m_sVSFilename.find_last_of('.')) + ".cso";

      file::File oVsFile(sVSFilename.c_str());
      file::File oPsFile(sPSFilename.c_str());      
      
      DX11_CHECK(pWindow->m_pDevice->CreateVertexShader(oVsFile.GetData(), oVsFile.GetSize(), nullptr, pRenderState->m_pVertexShader.ReleaseAndGetAddressOf()));      
      DX11_CHECK(pWindow->m_pDevice->CreatePixelShader(oPsFile.GetData(), oPsFile.GetSize(), nullptr, pRenderState->m_pPixelShader.ReleaseAndGetAddressOf()));

      const D3D11_INPUT_ELEMENT_DESC oInputDesc[] =
      {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							              D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	   0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
      };

      DX11_CHECK(pWindow->m_pDevice->CreateInputLayout(oInputDesc, 4u, oVsFile.GetData(), oVsFile.GetSize(), pRenderState->m_pInputLayout.ReleaseAndGetAddressOf()))

      D3D11_RASTERIZER_DESC oRasterizerDesc = {};
      oRasterizerDesc.FillMode = D3D11_FILL_SOLID;
      oRasterizerDesc.CullMode = D3D11_CULL_FRONT;
      oRasterizerDesc.FrontCounterClockwise = false;
      oRasterizerDesc.DepthBias = 0;// DEPTH_BIAS_D32_FLOAT(-0.00001);
      oRasterizerDesc.DepthBiasClamp = 0;//-0.001;
      oRasterizerDesc.SlopeScaledDepthBias = 0;// 1.f;
      oRasterizerDesc.DepthClipEnable = true;
      oRasterizerDesc.MultisampleEnable = true;
      oRasterizerDesc.AntialiasedLineEnable = true;      

      DX11_CHECK(pWindow->m_pDevice->CreateRasterizerState(&oRasterizerDesc, pRenderState->m_pRasterizer.GetAddressOf()))

      //BlendState.RenderTarget[0].SrcBlend = (D3D11_BLEND)m_srcBlendFactor;
      //BlendState.RenderTarget[0].DestBlend = (D3D11_BLEND)m_dstBlendFactor;
      //BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
      //BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
      //BlendState.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)m_blendOp;
      //BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      //BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

      D3D11_BLEND_DESC oBlendDesc = {};
      oBlendDesc.AlphaToCoverageEnable = false;
      oBlendDesc.IndependentBlendEnable = false;
      oBlendDesc.RenderTarget[0].BlendEnable = _oInfo.m_bBlendEnabled;
      oBlendDesc.RenderTarget[0].BlendOp = GetD3D11BlendOp( _oInfo.m_eBlendOp);      
      oBlendDesc.RenderTarget[0].DestBlend = GetD3D11Blend(_oInfo.m_eDstBlendFactor);
      oBlendDesc.RenderTarget[0].SrcBlend = GetD3D11Blend(_oInfo.m_eSrcBlendFactor);
      oBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      oBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
      oBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
      oBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;            

      DX11_CHECK(pWindow->m_pDevice->CreateBlendState(&oBlendDesc, pRenderState->m_pBlendState.GetAddressOf()));

      D3D11_DEPTH_STENCIL_DESC oDSDesc = {};
      oDSDesc.DepthEnable = _oInfo.m_bDepthWrite || _oInfo.m_bDepthRead;      
      oDSDesc.DepthWriteMask = _oInfo.m_bDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;            
      oDSDesc.DepthFunc = _oInfo.m_bDepthRead ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;      

      DX11_CHECK(pWindow->m_pDevice->CreateDepthStencilState(&oDSDesc, pRenderState->m_pDSState.GetAddressOf()));

      return pRenderState;
    }

    void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
    {
    }

    void EndRenderStateSetup()
    {
    }

    void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState)
    {
    }

    void BindAPIRenderState(APIRenderState* _pAPIRenderState)
    {
      APIWindow* pWindow = _pAPIRenderState->m_pOwnerWindow;

      pWindow->m_pContext->VSSetShader(_pAPIRenderState->m_pVertexShader.Get(), nullptr, 0u);
      pWindow->m_pContext->PSSetShader(_pAPIRenderState->m_pPixelShader.Get(), nullptr, 0u);
      pWindow->m_pContext->IASetInputLayout(_pAPIRenderState->m_pInputLayout.Get());
      pWindow->m_pContext->RSSetState(_pAPIRenderState->m_pRasterizer.Get());
      pWindow->m_pContext->OMSetBlendState(_pAPIRenderState->m_pBlendState.Get(), nullptr, 0xffffffff);
      pWindow->m_pContext->OMSetDepthStencilState(_pAPIRenderState->m_pDSState.Get(), 0);
    }

    void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
    {
      delete _pAPIRenderState;
    }

    // Render substate

    APIRenderSubState* CreateAPIRenderSubState()
    {
      return nullptr;
    }

    void BeginSubStateSetup(APIRenderSubState* _pAPIRenderState)
    {
    }

    void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo)
    {

      APIWindow* pWindow = _pCBuffer->m_pOwnerWindow;

      D3D11_BUFFER_DESC oDesc = {};
      oDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      oDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;
      oDesc.CPUAccessFlags = 0u;// D3D11_CPU_ACCESS_WRITE;
      oDesc.MiscFlags = 0u;
      oDesc.ByteWidth = _uSize;
      oDesc.StructureByteStride = 0;

      DX11_CHECK(pWindow->m_pDevice->CreateBuffer(&oDesc, NULL, _pCBuffer->m_pCBuffer.GetAddressOf()));

      _pCBuffer->m_stageMask = static_cast<unsigned int>(_oBindInfo.m_eStage);

      uint32_t uSlotOffset = 0u;

      if ((_pCBuffer->m_stageMask & static_cast<uint32_t>(PipelineStage::VERTEX)) != 0u)
      {
        switch (_oBindInfo.m_eLevel)
        {
        case ResourceFrequency::GLOBAL:
          uSlotOffset = VS_SLOT_OFFSET_GLOBAL;
          break;
        case ResourceFrequency::MATERIAL:
          uSlotOffset = VS_SLOT_OFFSET_MATERIAL;
          break;
        case ResourceFrequency::MATERIAL_INSTANCE:
          uSlotOffset = VS_SLOT_OFFSET_MATINSTANCE;
          break;
        default:
          uSlotOffset = VS_SLOT_OFFSET_MATINSTANCE;
        }
      }
      if ((_pCBuffer->m_stageMask & static_cast<uint32_t>(PipelineStage::PIXEL)) != 0u)
      {
        switch (_oBindInfo.m_eLevel)
        {
        case ResourceFrequency::GLOBAL:
          uSlotOffset = PS_SLOT_OFFSET_GLOBAL;
          break;
        case ResourceFrequency::MATERIAL:
          uSlotOffset = PS_SLOT_OFFSET_MATERIAL;
          break;
        case ResourceFrequency::MATERIAL_INSTANCE:
          uSlotOffset = PS_SLOT_OFFSET_MATINSTANCE;
          break;
        default:
          uSlotOffset = PS_SLOT_OFFSET_MATINSTANCE;
        }
      }

      _pCBuffer->m_slot = _oBindInfo.m_iBinding + uSlotOffset;      

    }

    void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
    {
      _pTexture->m_uSlot = _oBindInfo.m_iBinding;
      _pTexture->m_eStage = _oBindInfo.m_eStage;
    }

    void EndSubStateSetup()
    {
    }

    void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState)
    {
    }

    void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState)
    {
    }

    // Drawing

    int BeginDraw(APIWindow* _pWindow)
    {

      if (_pWindow->m_bResize)
      {
        ResizeSwapchain(_pWindow);
        _pWindow->m_bResize = false;
      }      
      
      BindAPIRenderTarget(_pWindow->m_pMsaaRenderTarget);

      return 0;
    }

    void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount, void* _pConstantData, uint32_t _uConstantSize)
    {

      APIWindow* pWindow = _pMesh->m_pOwnerWindow;

      UpdateAPIConstanBuffer(_pMesh->m_pModelCBuffer, _pConstantData, _uConstantSize);

      BindAPIConstantBuffer(_pMesh->m_pModelCBuffer);

      uint32_t uStride = sizeof(Vertex);
      uint32_t uOffset = 0u;

      pWindow->m_pContext->IASetVertexBuffers(0u, 1u, _pMesh->m_pVertexBuffer.GetAddressOf(), &uStride, &uOffset);
      pWindow->m_pContext->IASetIndexBuffer(_pMesh->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
      pWindow->m_pContext->DrawIndexed(_pMesh->m_uIndexCount, 0u, 0u);

    }

    void EndDraw(APIWindow* _pWindow)
    {

      ID3D11Texture2D* pBackBuffer = nullptr;
      DX11_CHECK(_pWindow->m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
      
      //_pWindow->m_pContext->CopyResource(pBackBuffer, _pWindow->m_pMsaaRenderTarget->m_pTexture.Get());
      _pWindow->m_pContext->ResolveSubresource(pBackBuffer, 0u, _pWindow->m_pMsaaRenderTarget->m_pTexture.Get(), 0u, _pWindow->m_eSwapchainFormat);

      DX11_CHECK(_pWindow->m_pSwapchain->Present(1u, 0u))

    }

  }
}