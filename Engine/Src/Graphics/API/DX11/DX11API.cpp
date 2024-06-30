#pragma once

#include "Graphics/API/DX11/DX11API.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Core/Exception.h"
#include "File/FileUtils.h"
#include "File/File.h"
#include "Util/WideStringUtil.h"
#include "Graphics/Mesh.h"
#include "Graphics/Vertex.h"
#include "Graphics/API/DX11/DX11Macros.h"
#include "Graphics/API/DX11/DX11Data.h"
#include "Graphics/API/DX11/APIWindow.h"
#include "Graphics/API/DX11/APICamera.h"
#include "Graphics/API/DX11/APIRenderState.h"
#include "Graphics/API/DX11/APIConstantBuffer.h"
#include "Graphics/API/DX11/APIMesh.h"
#include "Graphics/API/DX11/APITexture.h"
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

    void CreateDeviceAndSwapChain(APIWindow* _pWindow)
    {      
      DXGI_SWAP_CHAIN_DESC oSwapchainDesc;
      ZeroMemory(&oSwapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
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
    }

    void ResizeSwapchain(APIWindow* _pWindow)
    {
      PollWindowSize(_pWindow);

      _pWindow->m_pBackBufferRTV.ReleaseAndGetAddressOf();
      _pWindow->m_pDepthStencilView.ReleaseAndGetAddressOf();

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

      DXGI_FORMAT eDXGIFormat = GetDXGIFormat(_eFormat);

      D3D11_SUBRESOURCE_DATA data = {};
      data.pSysMem = _pData;
      data.SysMemPitch = uPixelSize * _uWidth;

      D3D11_TEXTURE2D_DESC desc = {};
      desc.ArraySize = 1;
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      desc.CPUAccessFlags = 0;
      desc.Format = eDXGIFormat;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Usage = D3D11_USAGE_IMMUTABLE;
      desc.MipLevels = 1;
      desc.Width = _uWidth;
      desc.Height = _uHeight;

      /*if (m_mipLevels != 1)
      {
        desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
      }*/

      //if (m_mipLevels == 1)
      {
        DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&desc, &data, pTexture->m_pTexture.GetAddressOf()));
      }
      /*else
      {
        GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf()));
      }*/

      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
      srvDesc.Format = eDXGIFormat;
      srvDesc.Texture2D.MipLevels = 1u;
      srvDesc.Texture2D.MostDetailedMip = 0u;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      DX11_CHECK(pWindow->m_pDevice->CreateShaderResourceView(pTexture->m_pTexture.Get(), &srvDesc, pTexture->m_pSRV.GetAddressOf()));

      /*if (m_mipLevels != 1)
      {
        GetContext()->UpdateSubresource(pTexture->m_pTexture.Get(), 0u, NULL, _pData, image.getMemPitch(), 0u);
        GetContext()->GenerateMips(m_srv.Get());
      }*/

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

    // Render state

    APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo)
    {

      APIRenderState* pRenderState = new APIRenderState();

      pRenderState->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

      APIWindow* pWindow = pRenderState->m_pOwnerWindow;

      //std::string sVsFilename(file::GetWorkingDirectory() + "Assets/Shaders/DX11/Vertex/VertexShader.cso");
      //std::string sPsFilename(file::GetWorkingDirectory() + "Assets/Shaders/DX11/Pixel/PixelShader.cso");

      //DX11_CHECK(D3DReadFileToBlob(CharToLPWSTR(sVsFilename.c_str()).c_str(), pRenderState->m_pBytecodeBlob.ReleaseAndGetAddressOf()));      
      file::File oVsFile("Assets/Shaders/DX11/Vertex/VertexShader.cso");
      file::File oPsFile("Assets/Shaders/DX11/Pixel/PixelShader.cso");
      
      DX11_CHECK(pWindow->m_pDevice->CreateVertexShader(oVsFile.GetData(), oVsFile.GetSize(), nullptr, pRenderState->m_pVertexShader.ReleaseAndGetAddressOf()));

      //Microsoft::WRL::ComPtr<ID3DBlob> pPsBlob;
      //DX11_CHECK(D3DReadFileToBlob(CharToLPWSTR(sPsFilename.c_str()).c_str(), pPsBlob.ReleaseAndGetAddressOf()));
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
      oRasterizerDesc.CullMode = D3D11_CULL_NONE;
      oRasterizerDesc.FrontCounterClockwise = false;
      oRasterizerDesc.DepthBias = 0;// DEPTH_BIAS_D32_FLOAT(-0.00001);
      oRasterizerDesc.DepthBiasClamp = 0;//-0.001;
      oRasterizerDesc.SlopeScaledDepthBias = 0;// 1.f;
      oRasterizerDesc.DepthClipEnable = true;
      oRasterizerDesc.MultisampleEnable = true;
      oRasterizerDesc.AntialiasedLineEnable = true;

      DX11_CHECK(pWindow->m_pDevice->CreateRasterizerState(&oRasterizerDesc, pRenderState->m_pRasterizer.GetAddressOf()))

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

      float aClearColor[] = { 0.f,0.f,0.f,1.f };
      unsigned int uClearFlags = D3D11_CLEAR_DEPTH;

      _pWindow->m_pContext->ClearRenderTargetView(_pWindow->m_pBackBufferRTV.Get(), aClearColor);
      _pWindow->m_pContext->ClearDepthStencilView(_pWindow->m_pDepthStencilView.Get(), uClearFlags, 1.0f, 0u);

      _pWindow->m_pContext->OMSetRenderTargets(1u, _pWindow->m_pBackBufferRTV.GetAddressOf(), _pWindow->m_pDepthStencilView.Get());

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

      DX11_CHECK(_pWindow->m_pSwapchain->Present(1u, 0u))

    }

  }
}