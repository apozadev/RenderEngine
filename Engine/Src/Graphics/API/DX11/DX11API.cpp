#pragma once

#ifdef RENDER_API_DX11

#include "Graphics/API/DX11/DX11API.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <dxgi.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "Core/Exception.h"
#include "File/FileUtils.h"
#include "File/InFile.h"
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
#include "Graphics/API/DX11/APIInternal.h"
#include "Graphics/API/DX11/DX11Pools.h"

namespace wrl = Microsoft::WRL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define CUSTOM_PROP_NAME "CustomWndProcData"

namespace api
{
  namespace dx11
  {    

    // Define the user data structure
    struct WndProcData
    {
      WNDPROC originalWndProc;  // The original WndProc
    };

    // Forward declaration of the custom WndProc
    LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void HookGLFWWndProc(GLFWwindow* window)
    {
      // Get the Win32 window handle from the GLFW window
      HWND hwnd = glfwGetWin32Window(window);

      // Allocate and set the WndProc data
      WndProcData* data = new WndProcData();
      data->originalWndProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);

      // Set the custom data as a property of the window
      SetProp(hwnd, CUSTOM_PROP_NAME, (HANDLE)data);

      // Hook the new WndProc
      SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);
    }

    void RestoreOriginalWndProc(GLFWwindow* window)
    {
      // Get the Win32 window handle from the GLFW window
      HWND hwnd = glfwGetWin32Window(window);

      // Retrieve the custom WndProc data
      WndProcData* data = (WndProcData*)GetProp(hwnd, CUSTOM_PROP_NAME);

      if (data)
      {
        // Restore the original WndProc
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)data->originalWndProc);

        // Clean up the custom data
        RemoveProp(hwnd, CUSTOM_PROP_NAME);
        delete data;
      }
    }

    LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
      // Retrieve the custom WndProc data
      WndProcData* data = (WndProcData*)GetProp(hWnd, CUSTOM_PROP_NAME);

      // Forward the event to ImGui's WndProc handler
      if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
      {
        return true;
      }

      // Call the original WndProc
      if (data && data->originalWndProc)
      {
        return CallWindowProc(data->originalWndProc, hWnd, msg, wParam, lParam);
      }

      // Default processing if no original WndProc exists
      return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    
    // General

    void InitializeAPI()
    {
      s_oGlobalData.m_uMaxMsaaSamples = 0u;
      s_oRenderStatePool.Initialize();
      s_oConstantBufferPool.Initialize();
      s_oMeshPool.Initialize();
      s_oTexturePool.Initialize();
      s_oRenderTargetPool.Initialize();

      std::vector<GlobalLayout::Resource>& lstCbuffs = s_oGlobalData.m_oGlobalLayout.m_lstCBuffers;
      lstCbuffs.push_back(GlobalLayout::Resource{ "GlobalBuffer", 0u, STAGE_VERTEX | STAGE_PIXEL });
      lstCbuffs.push_back(GlobalLayout::Resource{ "LightBuffer", 1u, STAGE_PIXEL});
      lstCbuffs.push_back(GlobalLayout::Resource{ "ModelBuffer", 3u, STAGE_VERTEX });

      std::vector<GlobalLayout::Resource>& lstTextures = s_oGlobalData.m_oGlobalLayout.m_lstTextures;
      lstTextures.push_back(GlobalLayout::Resource{ "Input0", 0u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Input1", 1u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Input2", 2u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Input3", 3u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Texture0", 4u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Texture1", 5u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Texture2", 6u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "Texture3", 7u, STAGE_PIXEL });      
      lstTextures.push_back(GlobalLayout::Resource{ "ShadowMap0", 8u, STAGE_PIXEL });      
      lstTextures.push_back(GlobalLayout::Resource{ "ShadowMap1", 9u, STAGE_PIXEL });      
      lstTextures.push_back(GlobalLayout::Resource{ "ShadowMap2", 10u, STAGE_PIXEL });      
      lstTextures.push_back(GlobalLayout::Resource{ "ShadowMap3", 11u, STAGE_PIXEL });      
      lstTextures.push_back(GlobalLayout::Resource{ "SpecEnvMap", 12u, STAGE_PIXEL });
      lstTextures.push_back(GlobalLayout::Resource{ "DiffEnvMap", 13u, STAGE_PIXEL });            
      lstTextures.push_back(GlobalLayout::Resource{ "BrdfLutTex", 14u, STAGE_PIXEL });            

      IMGUI_CHECKVERSION();
      ImGui::CreateContext();

      ImGuiIO& io = ImGui::GetIO(); (void)io;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls  
    }

    void ShutDownAPI()
    {

      ImGui::DestroyContext();

      s_oRenderStatePool.Clear();
      s_oConstantBufferPool.Clear();
      s_oMeshPool.Clear();
      s_oTexturePool.Clear();
      s_oRenderTargetPool.Clear();
    }

    uint32_t GetMaxMsaaSamples()
    {
      return s_oGlobalData.m_uMaxMsaaSamples;
    }

    // Window

    APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow, uint32_t _uMsaaSamples)
    {      

      APIWindow* pWindow = new APIWindow();  

      pWindow->m_pGlfwWindow = _pGlfwWindow;

      pWindow->m_hWnd = glfwGetWin32Window(_pGlfwWindow);

      pWindow->m_uNumSwapchainImages = 2u;

      pWindow->m_eSwapchainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;            
      
      PollWindowSize(pWindow);
      CreateDeviceAndSwapChain(pWindow);      
      
      if (s_oGlobalData.m_uMaxMsaaSamples < 1u)
      {
        s_oGlobalData.m_uMaxMsaaSamples = QueryMaxSupportedMsaaSamples(pWindow);
      }
      
      if (_uMsaaSamples > s_oGlobalData.m_uMaxMsaaSamples)
      {
        pWindow->m_uMsaaSamples = s_oGlobalData.m_uMaxMsaaSamples;
      }
      else
      {
        pWindow->m_uMsaaSamples = _uMsaaSamples;
      }

      CreateWindowRenderTarget(pWindow);

      // Default to triangle list just in case
      pWindow->m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // ImGui stuff
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();

      ImGui::StyleColorsDark(); 

      ImGui_ImplWin32_Init(glfwGetWin32Window(_pGlfwWindow));
      ImGui_ImplDX11_Init(pWindow->m_pDevice.Get(), pWindow->m_pContext.Get()); 

      ImGui::GetIO().DisplaySize.x = static_cast<float>(pWindow->m_uWidth);
      ImGui::GetIO().DisplaySize.y = static_cast<float>(pWindow->m_uHeight);

      HookGLFWWndProc(_pGlfwWindow);          

      SamplerConfig oSamplerCfg = {};
      oSamplerCfg.m_eAddressMode = TextureAddressMode::CLAMP;
      oSamplerCfg.m_eMagFilterMode = TextureFilterMode::POINT;
      oSamplerCfg.m_eMinFilterMode = TextureFilterMode::POINT;
      oSamplerCfg.m_eMipmapFilterMode = TextureFilterMode::POINT;

      uint8_t aDummyBuffer[4] = { 255u, 20u, 0u, 255u };
      void* pDummyBuffer = &(aDummyBuffer[0]);
      pWindow->m_pDummyTexture = CreateAPITexture(pWindow, &pDummyBuffer, 1u, 1u, ImageFormat::R8G8B8A8, 1u, 1u, TextureUsage::SHADER_RESOURCE, oSamplerCfg, false);

#ifndef NDEBUG
      Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
      DX11_CHECK(pWindow->m_pDevice.As(&d3dDebug))

      Microsoft::WRL::ComPtr<ID3D11InfoQueue> d3dInfoQueue;
      DX11_CHECK(d3dDebug.As(&d3dInfoQueue))      
      D3D11_MESSAGE_ID hide[] =
      {
        D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
      };
      D3D11_INFO_QUEUE_FILTER filter;
      memset(&filter, 0, sizeof(filter));
      filter.DenyList.NumIDs = _countof(hide);
      filter.DenyList.pIDList = hide;
      d3dInfoQueue->AddStorageFilterEntries(&filter);      
#endif

      return pWindow;
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

    uint32_t GetWindowMSAASamples(APIWindow* _pWindow)
    {
      return _pWindow->m_uMsaaSamples;
    }

    void ClearDefaultRenderTarget(APIWindow* _pWindow)
    {
      float aClearColor[] = { 0.f,0.f,0.f,1.f };
      unsigned int uClearFlags = D3D11_CLEAR_DEPTH;

      _pWindow->m_pContext->ClearRenderTargetView(_pWindow->m_pRtv.Get(), aClearColor);

      _pWindow->m_pContext->ClearDepthStencilView(_pWindow->m_pDsv.Get(), uClearFlags, 1.0f, 0u);
    }

    void BindDefaultRenderTarget(APIWindow* _pWindow)
    {      
      _pWindow->m_pContext->OMSetRenderTargets(1, _pWindow->m_pRtv.GetAddressOf(), _pWindow->m_pDsv.Get());

      D3D11_VIEWPORT oViewport;
      oViewport.Width = static_cast<FLOAT>(_pWindow->m_uWidth);
      oViewport.Height = static_cast<FLOAT>(_pWindow->m_uHeight);
      oViewport.MinDepth = 0.0f;
      oViewport.MaxDepth = 1.0f;
      oViewport.TopLeftX = 0.0f;
      oViewport.TopLeftY = 0.0f;

      _pWindow->m_pContext->RSSetViewports(1, &oViewport);
    }

    void UnbindDefaultRenderTarget(APIWindow* _pWindow)
    {

      ImGui::Render();
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

      ID3D11RenderTargetView* pNullView =  nullptr;      
      _pWindow->m_pContext->OMSetRenderTargets(1, &pNullView, nullptr);
    }

    void DestroyAPIWindow(APIWindow* _pAPIWindow)
    {

      RestoreOriginalWndProc(_pAPIWindow->m_pGlfwWindow);

      ImGui_ImplDX11_Shutdown();
      ImGui_ImplWin32_Shutdown();       

      DestroyAPITexture(_pAPIWindow, _pAPIWindow->m_pDummyTexture);

      _pAPIWindow->m_pRtv = nullptr;
      _pAPIWindow->m_pDsv = nullptr;
      _pAPIWindow->m_pColorTexture = nullptr;
      _pAPIWindow->m_pDSTexture = nullptr;      
      _pAPIWindow->m_pSwapchain = nullptr;      
      _pAPIWindow->m_pContext = nullptr;

      Microsoft::WRL::ComPtr<ID3D11Debug> pDebug;
      _pAPIWindow->m_pDevice.As(&pDebug);
      pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

      _pAPIWindow->m_pDevice = nullptr; 

      delete _pAPIWindow;
    }

    // Mesh

    APIMesh* CreateAPIMesh(const APIWindow* _pWindow, const void* _pVertexData, size_t _uVertexDataSize, const void* _pIndexData, size_t _uIndexDataSize)
    {

      APIMesh* pMesh = s_oMeshPool.PullElement();

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

      DX11_CHECK(_pWindow->m_pDevice->CreateBuffer(&oVBDesc, &oVBData, pMesh->m_pVertexBuffer.GetAddressOf()))

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

      DX11_CHECK(_pWindow->m_pDevice->CreateBuffer(&oIBDesc, &oIBData, pMesh->m_pIndexBuffer.GetAddressOf()))

      pMesh->m_uIndexCount = _uIndexDataSize / sizeof(uint16_t);

      // Model CBuffer
      pMesh->m_pModelCBuffer = CreateAPIConstantBuffer(_pWindow, sizeof(MeshConstant));

      ResourceBindInfo oBindInfo = {};
      oBindInfo.m_eLevel = ResourceFrequency::MATERIAL_INSTANCE;
      oBindInfo.m_uStageFlags = STAGE_VERTEX;
      oBindInfo.m_sName = "ModelBuffer";

      SubStateSetupConstantBuffer(_pWindow, pMesh->m_pModelCBuffer, sizeof(MeshConstant), oBindInfo);

      return pMesh;
    }

    void DestroyAPIMesh(const APIWindow* _pWindow, APIMesh* _pMesh)
    {
      DestroyAPIConstantBuffer(_pWindow, _pMesh->m_pModelCBuffer);
      s_oMeshPool.ReturnElement(_pMesh);
    }

    // Constant buffer

    APIConstantBuffer* CreateAPIConstantBuffer(const APIWindow* _pWindow, size_t _uSize)
    {
      APIConstantBuffer* pCBuffer = s_oConstantBufferPool.PullElement();

      D3D11_BUFFER_DESC oDesc = {};
      oDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      oDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;
      oDesc.CPUAccessFlags = 0u;// D3D11_CPU_ACCESS_WRITE;
      oDesc.MiscFlags = 0u;
      oDesc.ByteWidth = _uSize;
      oDesc.StructureByteStride = 0;

      DX11_CHECK(_pWindow->m_pDevice->CreateBuffer(&oDesc, NULL, pCBuffer->m_pCBuffer.GetAddressOf()));
      
      return pCBuffer;
    }

    void UpdateAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCbuffer, const void* _pData, size_t _uSize)
    {            
      /*D3D11_MAPPED_SUBRESOURCE oMsr;
      DX11_CHECK(pWindow->m_pContext->Map(_pCBuffer->m_pCBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &oMsr));

      memcpy(oMsr.pData, &_pData, _uSize);

      pWindow->m_pContext->Unmap(_pCBuffer->m_pCBuffer.Get(), 0u);*/

      _pWindow->m_pContext->UpdateSubresource(_pCbuffer->m_pCBuffer.Get(), 0u, nullptr, _pData, 0u, 0u);

    }

    void BindAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCbuffer)
    {
      if ((_pCbuffer->m_uStageFlags & STAGE_VERTEX) != 0u)
      {
        _pWindow->m_pContext->VSSetConstantBuffers(_pCbuffer->m_slot, 1u, _pCbuffer->m_pCBuffer.GetAddressOf());
      }
      if ((_pCbuffer->m_uStageFlags & STAGE_GEOM) != 0u)
      {
        _pWindow->m_pContext->GSSetConstantBuffers(_pCbuffer->m_slot, 1u, _pCbuffer->m_pCBuffer.GetAddressOf());
      }
      if ((_pCbuffer->m_uStageFlags & STAGE_PIXEL) != 0u)
      {
        _pWindow->m_pContext->PSSetConstantBuffers(_pCbuffer->m_slot, 1u, _pCbuffer->m_pCBuffer.GetAddressOf());
      }

    }

    void DestroyAPIConstantBuffer(const APIWindow* /*_pWindow*/, APIConstantBuffer* _pCBuffer)
    {
      s_oConstantBufferPool.ReturnElement(_pCBuffer);
    }

    // Texture

    APITexture* CreateAPITexture(const APIWindow* _pWindow, const void* const* _ppData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage, const SamplerConfig& _rSamplerConfig, bool _bIsCubemap)
    {
      APITexture* pTexture = s_oTexturePool.PullElement();      

      uint32_t uPixelSize = GetImageFormatSize(_eFormat);

      uint32_t uMemPitch = uPixelSize * _uWidth;

      DXGI_FORMAT eDXGIFormat = GetDXGIFormat(_eFormat);
      DXGI_FORMAT eDXGISrvFormat = GetDXGISrvFormat(_eFormat);

      uint32_t uMaxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_uWidth, _uWidth)))) + 1;
      pTexture->m_uMipLevels = _uMipLevels == 0u ? uMaxMipLevels : std::min(_uMipLevels, uMaxMipLevels);

      uint32_t uBindFlags = GetTextureUsage(_uUsage);
      if (pTexture->m_uMipLevels > 1u)
      {
        uBindFlags |= D3D11_BIND_RENDER_TARGET;
      }      

      uint32_t uLayers = _bIsCubemap ? 6u : 1u;

      D3D11_TEXTURE2D_DESC oTexDesc = {};
      oTexDesc.ArraySize = uLayers;
      oTexDesc.BindFlags = uBindFlags;
      oTexDesc.CPUAccessFlags = 0;
      oTexDesc.Format = eDXGIFormat;
      oTexDesc.SampleDesc.Count = _uMsaaSamples;
      oTexDesc.SampleDesc.Quality = 0;
      oTexDesc.Usage = D3D11_USAGE_DEFAULT;
      oTexDesc.MipLevels = pTexture->m_uMipLevels;
      oTexDesc.Width = _uWidth;
      oTexDesc.Height = _uHeight;      

      if (pTexture->m_uMipLevels != 1)
      {
        oTexDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
      }
      
      if(_bIsCubemap)
      {
        oTexDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
      }

      if (pTexture->m_uMipLevels == 1)
      {
        if (_bIsCubemap)
        {          
          D3D11_SUBRESOURCE_DATA aData[6];
          if (_ppData != nullptr)
          {
            for (int i = 0; i < 6; i++)
            {
              aData[i].pSysMem = _ppData[i];
              aData[i].SysMemPitch = uMemPitch;
            }
          }

          DX11_CHECK(_pWindow->m_pDevice->CreateTexture2D(&oTexDesc, _ppData ? &aData[0] : nullptr, pTexture->m_pTexture.GetAddressOf()));
        }
        else
        {
          D3D11_SUBRESOURCE_DATA oData = {};
          oData.pSysMem = _ppData ? *_ppData : nullptr;
          oData.SysMemPitch = uMemPitch;

          DX11_CHECK(_pWindow->m_pDevice->CreateTexture2D(&oTexDesc, _ppData ? &oData : nullptr, pTexture->m_pTexture.GetAddressOf()));
        }
      }
      else
      {
        //GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf()));
        DX11_CHECK(_pWindow->m_pDevice->CreateTexture2D(&oTexDesc, nullptr, pTexture->m_pTexture.GetAddressOf()));
      }

      D3D11_SHADER_RESOURCE_VIEW_DESC oSrvDesc = {};
      oSrvDesc.Format = eDXGISrvFormat;

      if (_bIsCubemap)
      {
        oSrvDesc.TextureCube.MipLevels = pTexture->m_uMipLevels;
        oSrvDesc.TextureCube.MostDetailedMip = 0u;
        oSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
      }
      else
      {                
        oSrvDesc.Texture2D.MipLevels = pTexture->m_uMipLevels;
        oSrvDesc.Texture2D.MostDetailedMip = 0u;
        oSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;        
      }

      DX11_CHECK(_pWindow->m_pDevice->CreateShaderResourceView(pTexture->m_pTexture.Get(), &oSrvDesc, pTexture->m_pSRV.GetAddressOf()));

      if (_ppData != nullptr && pTexture->m_uMipLevels != 1)
      {
        if (_bIsCubemap)
        {
          for (int i = 0; i < 6; i++)
          {
            UINT uSubResource = D3D11CalcSubresource(0, i, pTexture->m_uMipLevels);
            _pWindow->m_pContext->UpdateSubresource(pTexture->m_pTexture.Get(), uSubResource, nullptr, _ppData[i], uMemPitch, 0u);
          }
        }
        else
        {
          _pWindow->m_pContext->UpdateSubresource(pTexture->m_pTexture.Get(), 0u, nullptr, *_ppData, uMemPitch, 0u);
        }

        _pWindow->m_pContext->GenerateMips(pTexture->m_pSRV.Get());
      }

      pTexture->m_eFormat = eDXGIFormat;
      pTexture->m_iWidth = _uWidth;
      pTexture->m_iHeight = _uHeight;
      pTexture->m_uLayers = uLayers;

      D3D11_SAMPLER_DESC oSamplerDesc{};
      oSamplerDesc.AddressU = GetD3D11AdressMode(_rSamplerConfig.m_eAddressMode);
      oSamplerDesc.AddressV = GetD3D11AdressMode(_rSamplerConfig.m_eAddressMode);
      oSamplerDesc.AddressW = GetD3D11AdressMode(_rSamplerConfig.m_eAddressMode);
      oSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;//GetD3D11Filter(_rSamplerConfig.m_eMipmapFilterMode, _rSamplerConfig.m_eMinFilterMode, _rSamplerConfig.m_eMagFilterMode);
      oSamplerDesc.MaxAnisotropy = 16;
      oSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
      oSamplerDesc.MinLOD = 0;
      oSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;      

      _pWindow->m_pDevice->CreateSamplerState(&oSamplerDesc, pTexture->m_pSampler.GetAddressOf());

      return pTexture;
    }

    void GenerateMipMaps(const APIWindow* _pWindow, APITexture* _pTexture)
    {
      _pWindow->m_pContext->GenerateMips(_pTexture->m_pSRV.Get());
    }

    void BindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
    {
      if ((_pTexture->m_uStageFlags & STAGE_VERTEX) != 0u)
      {
        _pWindow->m_pContext->VSSetShaderResources(_pTexture->m_uSlot, 1u, _pTexture->m_pSRV.GetAddressOf());
        _pWindow->m_pContext->VSSetSamplers(_pTexture->m_uSlot, 1u, _pTexture->m_pSampler.GetAddressOf());
      }
      if ((_pTexture->m_uStageFlags & STAGE_GEOM) != 0u)
      {
        _pWindow->m_pContext->GSSetShaderResources(_pTexture->m_uSlot, 1u, _pTexture->m_pSRV.GetAddressOf());
        _pWindow->m_pContext->GSSetSamplers(_pTexture->m_uSlot, 1u, _pTexture->m_pSampler.GetAddressOf());
      }
      if ((_pTexture->m_uStageFlags & STAGE_PIXEL) != 0u)
      {
        _pWindow->m_pContext->PSSetShaderResources(_pTexture->m_uSlot, 1u, _pTexture->m_pSRV.GetAddressOf());
        _pWindow->m_pContext->PSSetSamplers(_pTexture->m_uSlot, 1u, _pTexture->m_pSampler.GetAddressOf());
      }
    }

    void UnbindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
    {
      ID3D11ShaderResourceView* pNullSrv = nullptr;

      if ((_pTexture->m_uStageFlags & STAGE_VERTEX) != 0u)
      {
        _pWindow->m_pContext->VSSetShaderResources(_pTexture->m_uSlot, 1u, &pNullSrv);
      }
      if ((_pTexture->m_uStageFlags & STAGE_GEOM) != 0u)
      {
        _pWindow->m_pContext->GSSetShaderResources(_pTexture->m_uSlot, 1u, &pNullSrv);
      }
      if ((_pTexture->m_uStageFlags & STAGE_PIXEL) != 0u)
      {
        _pWindow->m_pContext->PSSetShaderResources(_pTexture->m_uSlot, 1u, &pNullSrv);
      }
    }

    void ClearAPITexture(const APIWindow* /*_pWindow*/, APITexture* /*_pTexture*/, TextureUsage /*_eUsage*/)
    {

    }

    void BlitAPITexture(const APIWindow* _pWindow, APITexture* _pSrcTexture, const TextureSubResource& _rSrcSubResource, APITexture* _pDstTexture, const TextureSubResource& _rDstSubResource)
    {

      if (_rSrcSubResource.m_uMipLevels != _rDstSubResource.m_uMipLevels || _rSrcSubResource.m_uLayers != _rDstSubResource.m_uLayers)
      {
        THROW_GENERIC_EXCEPTION("BlitAPITexture: Subresource Mip level count and layer count must match");
      }
      else if ((_rSrcSubResource.m_uBaseMip + _rSrcSubResource.m_uMipLevels) > _pSrcTexture->m_uMipLevels &&
        (_rDstSubResource.m_uBaseMip + _rDstSubResource.m_uMipLevels) > _pDstTexture->m_uMipLevels)
      {
        THROW_GENERIC_EXCEPTION("BlitAPITexture: Subresource Mip levels are outside of the texure mip level range");
      }
      else if ((_rSrcSubResource.m_uBaseLayer + _rSrcSubResource.m_uLayers) > _pSrcTexture->m_uLayers &&
        (_rDstSubResource.m_uBaseLayer + _rDstSubResource.m_uLayers) > _pDstTexture->m_uLayers)
      {
        THROW_GENERIC_EXCEPTION("BlitAPITexture: Subresource array layers are outside of the texure array layers range");
      }

      for (uint32_t uMipLevel = 0; uMipLevel < _rSrcSubResource.m_uMipLevels; uMipLevel++)
      {
        for (uint32_t uLayer = 0u; uLayer < _rSrcSubResource.m_uLayers; uLayer++)
        {
          uint32_t uSrcSubRes = D3D11CalcSubresource(uMipLevel + _rSrcSubResource.m_uBaseMip, uLayer + _rSrcSubResource.m_uBaseLayer, _pSrcTexture->m_uMipLevels);
          uint32_t uDstSubRes = D3D11CalcSubresource(uMipLevel + _rDstSubResource.m_uBaseMip, uLayer + _rDstSubResource.m_uBaseLayer, _pDstTexture->m_uMipLevels);

          _pWindow->m_pContext->CopySubresourceRegion(
            _pDstTexture->m_pTexture.Get(),
            uDstSubRes,
            0u, 0u, 0u,
            _pSrcTexture->m_pTexture.Get(),
            uSrcSubRes,
            nullptr);
        }
      }
    }

    void DestroyAPITexture(const APIWindow* /*_pWindow*/, APITexture* _pTexture)
    {
      s_oTexturePool.ReturnElement(_pTexture);
    }

    // RenderTarget

    APIRenderTarget* CreateAPIRenderTarget()
    {
      APIRenderTarget* pRenderTarget = s_oRenderTargetPool.PullElement();

      return pRenderTarget;
    }

    void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples, bool _bIsCubemap)
    {
      s_oGlobalData.m_oRenderTargetBuilder.m_uWidth = _uWidth;
      s_oGlobalData.m_oRenderTargetBuilder.m_uHeight = _uHeight;
      s_oGlobalData.m_oRenderTargetBuilder.m_eColorFormat = GetDXGIFormat(_eFormat);
      s_oGlobalData.m_oRenderTargetBuilder.m_eDepthStencilFormat = GetDXGIFormat(_eDepthStencilFormat);
      s_oGlobalData.m_oRenderTargetBuilder.m_uMsaaSamples = _uMsaaSamples;
      s_oGlobalData.m_oRenderTargetBuilder.m_bIsCubemap = _bIsCubemap;
      s_oGlobalData.m_oRenderTargetBuilder.m_pRenderTarget = _pRenderTarget;
    }

    void RenderTargetAddColorTexture(APITexture* _pTexture, uint32_t _uMipLevel)
    {            
      s_oGlobalData.m_oRenderTargetBuilder.AddColorTexture(_pTexture, _uMipLevel);
    }

    void RenderTargetSetDepthStencilTexture(APITexture* _pTexture, uint32_t _uMipLevel)
    {            
      s_oGlobalData.m_oRenderTargetBuilder.SetDepthTexture(_pTexture, _uMipLevel);
    }

    void RenderTargetAddColorResolveTexture(APITexture* _pTexture)
    {
      s_oGlobalData.m_oRenderTargetBuilder.AddResolveColorTexture(_pTexture);
    }

    void EndRenderTargetSetup(const APIWindow* _pWindow)
    {
      s_oGlobalData.m_oRenderTargetBuilder.Build(_pWindow);
      s_oGlobalData.m_oRenderTargetBuilder.Clear();
    }

    void ClearAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
    {
      float aClearColor[] = { 0.f,0.f,0.f,1.f };
      unsigned int uClearFlags = D3D11_CLEAR_DEPTH;

      for (auto& pRtv : _pRenderTarget->m_lstRtv)
      {
        _pWindow->m_pContext->ClearRenderTargetView(pRtv.Get(), aClearColor);
      }

      if (_pRenderTarget->m_pDsv.Get())
      {
        _pWindow->m_pContext->ClearDepthStencilView(_pRenderTarget->m_pDsv.Get(), uClearFlags, 1.0f, 0u);
      }

    }

    void BindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
    {   
      if (_pRenderTarget->m_lstRtv.size() > 0u)
      {
        _pWindow->m_pContext->OMSetRenderTargets(_pRenderTarget->m_lstRtv.size(), _pRenderTarget->m_lstRtv[0].GetAddressOf(), _pRenderTarget->m_pDsv.Get());
      }
      else
      {
        _pWindow->m_pContext->OMSetRenderTargets(0u, nullptr, _pRenderTarget->m_pDsv.Get());
      }

      D3D11_VIEWPORT oViewport;
      oViewport.Width = static_cast<FLOAT>(_pRenderTarget->m_iWidth);
      oViewport.Height = static_cast<FLOAT>(_pRenderTarget->m_iHeight);
      oViewport.MinDepth = 0.0f;
      oViewport.MaxDepth = 1.0f;
      oViewport.TopLeftX = 0.0f;
      oViewport.TopLeftY = 0.0f;

      _pWindow->m_pContext->RSSetViewports(1, &oViewport);
    }

    void UnbindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
    {

      //const std::vector<ID3D11RenderTargetView*> lstNullViews(_pRenderTarget->m_lstRtv.size(), nullptr);
      //s_oGlobalData.m_pUsingWindow->m_pContext->OMSetRenderTargets(_pRenderTarget->m_lstRtv.size(), lstNullViews.data(), nullptr);

      if (_pRenderTarget->m_lstRtv.size() > 0u)
      {
        const std::vector<ID3D11RenderTargetView*> lstNullViews(_pRenderTarget->m_lstRtv.size(), nullptr);
        _pWindow->m_pContext->OMSetRenderTargets(_pRenderTarget->m_lstRtv.size(), lstNullViews.data(), nullptr);
      }
      else
      {
        _pWindow->m_pContext->OMSetRenderTargets(0u, nullptr, nullptr);
      }

      _pWindow = nullptr;
    }
    
    void DestroyAPIRenderTarget(const APIWindow* /*_pWindow*/, APIRenderTarget* _pRenderTarget)
    {
      s_oRenderTargetPool.ReturnElement(_pRenderTarget);
    }

    // Render state

    APIRenderState* CreateAPIRenderState(const APIWindow* _pWindow, const RenderStateInfo& _oInfo, const APIRenderTarget* /*_pRenderTarget*/, uint32_t _uMsaaSamples)
    {

      APIRenderState* pRenderState = s_oRenderStatePool.PullElement();

      size_t uSlashPosVS = _oInfo.m_sVSFilename.find_last_of('/');      
      size_t uSlashPosPS = _oInfo.m_sPSFilename.find_last_of('/');

      size_t uDotPosVS = _oInfo.m_sVSFilename.find_last_of('.');      
      size_t uDotPosPS = _oInfo.m_sPSFilename.find_last_of('.');

      std::string sVSPath = _oInfo.m_sVSFilename.substr(0, uSlashPosVS) + "/DX11";      
      std::string sPSPath = _oInfo.m_sPSFilename.substr(0, uSlashPosPS) + "/DX11";

      std::string sVSFilename = _oInfo.m_sVSFilename.substr(uSlashPosVS, uDotPosVS - uSlashPosVS) + ".cso";      
      std::string sPSFilename = _oInfo.m_sPSFilename.substr(uSlashPosPS, uDotPosPS - uSlashPosPS) + ".cso";

      file::InFile oVsFile((sVSPath + sVSFilename).c_str());      
      file::InFile oPsFile((sPSPath + sPSFilename).c_str());
      
      DX11_CHECK(_pWindow->m_pDevice->CreateVertexShader(oVsFile.GetData(), oVsFile.GetSize(), nullptr, pRenderState->m_pVertexShader.ReleaseAndGetAddressOf()));            
      DX11_CHECK(_pWindow->m_pDevice->CreatePixelShader(oPsFile.GetData(), oPsFile.GetSize(), nullptr, pRenderState->m_pPixelShader.ReleaseAndGetAddressOf()));      

      DX11_CHECK(D3DReflect(oVsFile.GetData(), oVsFile.GetSize(), IID_ID3D11ShaderReflection, (void**)pRenderState->m_pVertexReflection.ReleaseAndGetAddressOf()));
      DX11_CHECK(D3DReflect(oPsFile.GetData(), oPsFile.GetSize(), IID_ID3D11ShaderReflection, (void**)pRenderState->m_pPixelReflection.ReleaseAndGetAddressOf()));

      pRenderState->m_pGeometryShader = nullptr;

      if (!_oInfo.m_sGSFilename.empty())
      {
        size_t uSlashPosGS = _oInfo.m_sGSFilename.find_last_of('/');        
        size_t uDotPosGS = _oInfo.m_sGSFilename.find_last_of('.');
        std::string sGSPath = _oInfo.m_sGSFilename.empty() ? "" : _oInfo.m_sGSFilename.substr(0, uSlashPosGS) + "/DX11";
        std::string sGSFilename = _oInfo.m_sGSFilename.empty() ? "" : _oInfo.m_sGSFilename.substr(uSlashPosGS, uDotPosGS - uSlashPosGS) + ".cso";

        file::InFile oGsFile(_oInfo.m_sGSFilename.empty() ? "" : (sGSPath + sGSFilename).c_str());

        DX11_CHECK(_pWindow->m_pDevice->CreateGeometryShader(oGsFile.GetData(), oGsFile.GetSize(), nullptr, pRenderState->m_pGeometryShader.ReleaseAndGetAddressOf()));

        DX11_CHECK(D3DReflect(oGsFile.GetData(), oGsFile.GetSize(), IID_ID3D11ShaderReflection, (void**)pRenderState->m_pGeometryReflection.ReleaseAndGetAddressOf()));
      }

      const D3D11_INPUT_ELEMENT_DESC oInputDesc[] =
      {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							              D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	   0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
      };

      DX11_CHECK(_pWindow->m_pDevice->CreateInputLayout(oInputDesc, 5u, oVsFile.GetData(), oVsFile.GetSize(), pRenderState->m_pInputLayout.ReleaseAndGetAddressOf()))

      D3D11_RASTERIZER_DESC oRasterizerDesc = {};
      oRasterizerDesc.FillMode = D3D11_FILL_SOLID;
      oRasterizerDesc.CullMode = GetD3D11CullMode(_oInfo.m_eCullMode);
      oRasterizerDesc.FrontCounterClockwise = true;
      oRasterizerDesc.DepthBias = 0;// DEPTH_BIAS_D32_FLOAT(-0.00001);
      oRasterizerDesc.DepthBiasClamp = 0;//-0.001;
      oRasterizerDesc.SlopeScaledDepthBias = 0;// 1.f;
      oRasterizerDesc.DepthClipEnable = true;
      oRasterizerDesc.MultisampleEnable = _uMsaaSamples > 1u;
      oRasterizerDesc.AntialiasedLineEnable = true;      

      DX11_CHECK(_pWindow->m_pDevice->CreateRasterizerState(&oRasterizerDesc, pRenderState->m_pRasterizer.GetAddressOf()))

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

      DX11_CHECK(_pWindow->m_pDevice->CreateBlendState(&oBlendDesc, pRenderState->m_pBlendState.GetAddressOf()));

      D3D11_DEPTH_STENCIL_DESC oDSDesc = {};
      oDSDesc.DepthEnable = _oInfo.m_bDepthWrite || _oInfo.m_bDepthRead;      
      oDSDesc.DepthWriteMask = _oInfo.m_bDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;            

      if (_oInfo.m_bDepthRead)
      {
        oDSDesc.DepthFunc = GetD3D11DepthCompareFunc(_oInfo.m_eDepthCompareOp);
      }
      else
      {
        oDSDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
      }
      

      DX11_CHECK(_pWindow->m_pDevice->CreateDepthStencilState(&oDSDesc, pRenderState->m_pDSState.GetAddressOf()));

      return pRenderState;
    }

    void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
    {
    }

    void RenderStateSetupConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState)
    {      
      _pCBuffer->m_uStageFlags = _oBindInfo.m_uStageFlags;

      _pCBuffer->m_slot = 0xFFFFFFFF;

      if ((_oBindInfo.m_uStageFlags & STAGE_VERTEX) != 0u)
      {
        ID3D11ShaderReflection* pReflection = _pRenderState->m_pVertexReflection.Get();
        _pCBuffer->m_slot = GetBindFromReflection(pReflection, _oBindInfo.m_sName);
      }
      if ((_oBindInfo.m_uStageFlags & STAGE_GEOM) != 0u)
      {
        if (ID3D11ShaderReflection* pReflection = _pRenderState->m_pGeometryReflection.Get())
        {
          _pCBuffer->m_slot = GetBindFromReflection(pReflection, _oBindInfo.m_sName);
        }
      }
      if (_pCBuffer->m_slot == 0xFFFFFFFF && (_oBindInfo.m_uStageFlags & STAGE_PIXEL) != 0u)
      {
        ID3D11ShaderReflection* pReflection = _pRenderState->m_pPixelReflection.Get();
        _pCBuffer->m_slot = GetBindFromReflection(pReflection, _oBindInfo.m_sName);
      }

      if (_pCBuffer->m_slot != 0xFFFFFFFF)
      {
        return;
      }
    }

    void RenderStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState)
    {
      _pTexture->m_uStageFlags = _oBindInfo.m_uStageFlags;

      /*switch (_oBindInfo.m_eStage)
      {
      case STAGE_VERTEX:
        s_oGlobalData.m_pUsingRenderState->m_pVertexReflection->
        break;
      case STAGE_PIXEL:
        break;
      }*/
    }

    void EndRenderStateSetup(const APIWindow* _pWindow)
    {
    }

    void BindAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState)
    {
      _pWindow->m_pContext->VSSetShader(_pAPIRenderState->m_pVertexShader.Get(), nullptr, 0u);
      _pWindow->m_pContext->PSSetShader(_pAPIRenderState->m_pPixelShader.Get(), nullptr, 0u);      
      _pWindow->m_pContext->GSSetShader(_pAPIRenderState->m_pGeometryShader.Get(), nullptr, 0u);      
      _pWindow->m_pContext->IASetInputLayout(_pAPIRenderState->m_pInputLayout.Get());
      _pWindow->m_pContext->RSSetState(_pAPIRenderState->m_pRasterizer.Get());
      _pWindow->m_pContext->OMSetBlendState(_pAPIRenderState->m_pBlendState.Get(), nullptr, 0xffffffff);
      _pWindow->m_pContext->OMSetDepthStencilState(_pAPIRenderState->m_pDSState.Get(), 0);
    }

    void DestroyAPIRenderState(const APIWindow* /*_pWindow*/, APIRenderState* _pAPIRenderState)
    {
      s_oRenderStatePool.ReturnElement(_pAPIRenderState);
    }

    // Shader Reflection

    uint32_t GetConstantBufferCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage)
    {
      uint32_t uCount = 0u;

      ID3D11ShaderReflection* pReflection = nullptr;

      switch (_eStage)
      {
      case STAGE_VERTEX:        
        pReflection = _pAPIRenderState->m_pVertexReflection.Get();
        break;
      case STAGE_GEOM:
        pReflection = _pAPIRenderState->m_pGeometryReflection.Get();
        break;
      case STAGE_PIXEL:
        pReflection = _pAPIRenderState->m_pPixelReflection.Get();
        break;
      default:
        break;
      }

      if (pReflection)
      {
        D3D11_SHADER_DESC oDesc = {};
        pReflection->GetDesc(&oDesc);

        for(uint32_t i = 0; i < oDesc.ConstantBuffers; i++)
        {
          ID3D11ShaderReflectionConstantBuffer* pCBuffer = pReflection->GetConstantBufferByIndex(i);

          D3D11_SHADER_BUFFER_DESC oBufferDesc;
          pCBuffer->GetDesc(&oBufferDesc);

          if (!IsGlobalResource(oBufferDesc.Name, _eStage))
          {
            uCount++;
          }
        }
      }

      return uCount;
    }

    uint32_t GetTextureCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage)
    {      

      uint32_t uTextureCount = 0u;

      ID3D11ShaderReflection* pReflection = nullptr;

      switch (_eStage)
      {
      case STAGE_VERTEX:        
        pReflection  = _pAPIRenderState->m_pVertexReflection.Get();
        break;
      case STAGE_GEOM:
        pReflection = _pAPIRenderState->m_pGeometryReflection.Get();
        break;
      case STAGE_PIXEL:
        pReflection = _pAPIRenderState->m_pPixelReflection.Get();
        break;
      default:
        break;
      }

      if (pReflection)
      {
        D3D11_SHADER_DESC oDesc = {};
        pReflection->GetDesc(&oDesc);

        for (unsigned int i = 0u; i < oDesc.BoundResources; i++)
        {
          D3D11_SHADER_INPUT_BIND_DESC oResourceDesc;
          pReflection->GetResourceBindingDesc(i, &oResourceDesc);

          if (oResourceDesc.Type == D3D_SIT_TEXTURE && !IsGlobalResource(oResourceDesc.Name, _eStage))
          {
            uTextureCount++;;
          }
        }
      }

      return uTextureCount;
    }

    std::string GetConstantBufferName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
    {
      D3D11_SHADER_BUFFER_DESC oDesc{};
      
      if(GetBufferByIndex(_pAPIRenderState, _eStage, _uIdx, nullptr, oDesc))
      {
        return oDesc.Name;
      }
              
      else
      {
        return "[ERROR]";        
      }
    }

    const char* GetTextureName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
    {
      return nullptr;
    }

    uint32_t GetConstantBufferMemberCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
    {
      D3D11_SHADER_BUFFER_DESC oDesc{};

      if (GetBufferByIndex(_pAPIRenderState, _eStage, _uIdx, nullptr, oDesc))
      {
        return oDesc.Variables;
      }

      return 0u;
    }

    std::string GetConstantBufferMemberName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
    {
      D3D11_SHADER_BUFFER_DESC oBufferDesc = {};

      ID3D11ShaderReflectionConstantBuffer* pCBuffer = nullptr;
      
      if (GetBufferByIndex(_pAPIRenderState, _eStage, _uIdx, &pCBuffer, oBufferDesc))
      {
        if (_uMemberIdx < oBufferDesc.Variables)
        {
          if (ID3D11ShaderReflectionVariable* pVariable = pCBuffer->GetVariableByIndex(_uMemberIdx))
          {
            D3D11_SHADER_VARIABLE_DESC oVarDesc = {};
            pVariable->GetDesc(&oVarDesc);

            return oVarDesc.Name;
          }
        }
      }

      return "[ERROR]";
    }

    size_t GetConstantBufferMemberSize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
    {
      D3D11_SHADER_BUFFER_DESC oBufferDesc = {};

      ID3D11ShaderReflectionConstantBuffer* pCBuffer = nullptr;

      if (GetBufferByIndex(_pAPIRenderState, _eStage, _uIdx, &pCBuffer, oBufferDesc))
      {
        if (_uMemberIdx < oBufferDesc.Variables)
        {
          if (ID3D11ShaderReflectionVariable* pVariable = pCBuffer->GetVariableByIndex(_uMemberIdx))
          {
            D3D11_SHADER_VARIABLE_DESC oVarDesc = {};
            pVariable->GetDesc(&oVarDesc);

            return oVarDesc.Size;
          }
        }
      }

      return 0u;
    }

    ConstantBufferType GetConstantBufferMemberType(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
    {

      D3D11_SHADER_BUFFER_DESC oBufferDesc = {};

      ID3D11ShaderReflectionConstantBuffer* pCBuffer = nullptr;

      if (GetBufferByIndex(_pAPIRenderState, _eStage, _uIdx, &pCBuffer, oBufferDesc))
      {
        if (_uMemberIdx < oBufferDesc.Variables)
        {
          if (ID3D11ShaderReflectionVariable* pVariable = pCBuffer->GetVariableByIndex(_uMemberIdx))
          {
            D3D11_SHADER_VARIABLE_DESC oVarDesc = {};
            pVariable->GetDesc(&oVarDesc);

            ID3D11ShaderReflectionType* pType = pVariable->GetType();
            D3D11_SHADER_TYPE_DESC oTypeDesc;
            pType->GetDesc(&oTypeDesc);

            switch (oTypeDesc.Columns)
            {
            case 1u:
              return ConstantBufferType::SCALAR;
            case 2u:
              return ConstantBufferType::VEC2;
            case 3u:
              if (oTypeDesc.Rows == 3u)
              {
                return ConstantBufferType::MAT3;
              }
              else if (oTypeDesc.Rows == 1u)
              {
                return ConstantBufferType::VEC3;
              }
            case 4u:
              if (oTypeDesc.Rows == 4u)
              {
                return ConstantBufferType::MAT4;
              }
              else if (oTypeDesc.Rows == 1u)
              {
                return ConstantBufferType::VEC4;
              }
            default:
              break;
            }
          }
        }
      }

      return ConstantBufferType::NONE;
    }

    uint32_t GetConstantBufferMemberArraySize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
    {
      uint32_t uSize = 0u;

      D3D11_SHADER_BUFFER_DESC oBufferDesc = {};

      ID3D11ShaderReflectionConstantBuffer* pCBuffer = nullptr;

      if (GetBufferByIndex(_pAPIRenderState, _eStage, _uIdx, &pCBuffer, oBufferDesc))
      {
        if (_uMemberIdx < oBufferDesc.Variables)
        {
          if (ID3D11ShaderReflectionVariable* pVariable = pCBuffer->GetVariableByIndex(_uMemberIdx))
          {
            D3D11_SHADER_VARIABLE_DESC oVarDesc = {};
            pVariable->GetDesc(&oVarDesc);

            ID3D11ShaderReflectionType* pType = pVariable->GetType();
            D3D11_SHADER_TYPE_DESC oTypeDesc;
            pType->GetDesc(&oTypeDesc);

            uSize = oTypeDesc.Elements;
          }
        }
      }

      if (uSize < 1)
      {
        uSize = 1;
      }

      return uSize;
    }

    // Render substate

    APIRenderSubState* CreateAPIRenderSubState(const APIWindow* /*_pWindow*/, ResourceFrequency /*_eFrequency*/)
    {
      return nullptr;
    }

    void BeginSubStateSetup(const APIWindow* /*_pWindow*/, APIRenderSubState* /*_pAPIRenderSubState*/, ResourceFrequency /*_eFrequency*/)
    {
    }

    void SubStateSetupConstantBuffer(const APIWindow* /*_pWindow*/, APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo)
    {      
      _pCBuffer->m_uStageFlags = _oBindInfo.m_uStageFlags;
      
      for (GlobalLayout::Resource& rGlobalCBuff : s_oGlobalData.m_oGlobalLayout.m_lstCBuffers)
      {
        if (rGlobalCBuff.m_sName == _oBindInfo.m_sName
          && rGlobalCBuff.m_uStageFlags == _oBindInfo.m_uStageFlags)
        {
          _pCBuffer->m_slot = rGlobalCBuff.m_uBinding;
          return;
        }
      }      

      THROW_GENERIC_EXCEPTION("[API] Cbuffer name not found")
    }

    void SubStateSetupTexture(const APIWindow* /*_pWindow*/, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
    {      
      _pTexture->m_uStageFlags = _oBindInfo.m_uStageFlags;

      for (GlobalLayout::Resource& rGlobalTexture : s_oGlobalData.m_oGlobalLayout.m_lstTextures)
      {
        if (rGlobalTexture.m_sName == _oBindInfo.m_sName
          && rGlobalTexture.m_uStageFlags == _oBindInfo.m_uStageFlags)
        {
          _pTexture->m_uSlot = rGlobalTexture.m_uBinding;
          return;
        }
      }      

      THROW_GENERIC_EXCEPTION("[API] Texture name not found")

    }

    void EndSubStateSetup(const APIWindow* /*_pWindow*/)
    {
    }

    void BindAPIRenderSubState(const APIWindow* /*_pWindow*/, APIRenderState* /*_pRenderState*/, APIRenderSubState* /*_pAPIRenderSubState*/, ResourceFrequency /*_eFrequency*/)
    {
    }

    void DestroyRenderSubState(const APIWindow* /*_pWindow*/, APIRenderSubState* /*_pAPIRenderSubState*/)
    {
    }

    // Drawing

    void WaitForLastImage(const APIWindow* /*_pWindow*/)
    {
      // We dont really need this feature, I think. DX11 should manage synchronization automatically
    }

    int BeginDraw(APIWindow* _pWindow)
    {

      if (_pWindow->m_bResize)
      {
        ImGui_ImplDX11_InvalidateDeviceObjects();

        ResizeSwapchain(_pWindow);

        ImGui_ImplDX11_CreateDeviceObjects();

        ImGui::EndFrame();

        ImGui::GetIO().DisplaySize.x = static_cast<float>(_pWindow->m_uWidth);
        ImGui::GetIO().DisplaySize.y = static_cast<float>(_pWindow->m_uHeight);

        _pWindow->m_bResize = false;

        return 1;
      }        

      // Bind dummy texture for all slots. We will override later if used
      // Not very efficient...
      for (auto& rResource : s_oGlobalData.m_oGlobalLayout.m_lstTextures)
      {
        if ((rResource.m_uStageFlags & STAGE_VERTEX) != 0)
        {
          _pWindow->m_pContext->VSSetShaderResources(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSRV.GetAddressOf());
          _pWindow->m_pContext->VSSetSamplers(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSampler.GetAddressOf());
        }
        if ((rResource.m_uStageFlags & STAGE_GEOM) != 0)
        {
          _pWindow->m_pContext->GSSetShaderResources(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSRV.GetAddressOf());
          _pWindow->m_pContext->GSSetSamplers(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSampler.GetAddressOf());
        }
        if ((rResource.m_uStageFlags & STAGE_PIXEL) != 0)
        {
          _pWindow->m_pContext->PSSetShaderResources(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSRV.GetAddressOf());
          _pWindow->m_pContext->PSSetSamplers(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSampler.GetAddressOf());
        }
      }      
      
      return 0;
    }

    void BeginDrawOffline(APIWindow* _pWindow)
    {
      // Bind dummy texture for all slots. We will override later if used
      // Not very efficient...
      for (auto& rResource : s_oGlobalData.m_oGlobalLayout.m_lstTextures)
      {
        if ((rResource.m_uStageFlags & STAGE_VERTEX) != 0)
        {
          _pWindow->m_pContext->VSSetShaderResources(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSRV.GetAddressOf());
          _pWindow->m_pContext->VSSetSamplers(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSampler.GetAddressOf());
        }
        if ((rResource.m_uStageFlags & STAGE_GEOM) != 0)
        {
          _pWindow->m_pContext->GSSetShaderResources(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSRV.GetAddressOf());
          _pWindow->m_pContext->GSSetSamplers(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSampler.GetAddressOf());
        }
        if ((rResource.m_uStageFlags & STAGE_PIXEL) != 0)
        {
          _pWindow->m_pContext->PSSetShaderResources(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSRV.GetAddressOf());
          _pWindow->m_pContext->PSSetSamplers(rResource.m_uBinding, 1u, _pWindow->m_pDummyTexture->m_pSampler.GetAddressOf());
        }
      }
    }

    void DrawMesh(const APIWindow* _pWindow, APIRenderState* _pRenderState, APIMesh* _pMesh, uint32_t _uIndexCount, const void* _pConstantData, uint32_t _uConstantSize)
    {
      UpdateAPIConstantBuffer(_pWindow, _pMesh->m_pModelCBuffer, _pConstantData, _uConstantSize);

      BindAPIConstantBuffer(_pWindow, _pMesh->m_pModelCBuffer);

      uint32_t uStride = sizeof(Vertex);
      uint32_t uOffset = 0u;

      _pWindow->m_pContext->IASetVertexBuffers(0u, 1u, _pMesh->m_pVertexBuffer.GetAddressOf(), &uStride, &uOffset);
      _pWindow->m_pContext->IASetIndexBuffer(_pMesh->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
      _pWindow->m_pContext->DrawIndexed(_pMesh->m_uIndexCount, 0u, 0u);

    }

    void EndDraw(APIWindow* /*_pWindow*/)
    {
      // Nothing to do...
    }

    void Present(APIWindow* _pWindow)
    {
      if (_pWindow->m_pColorTexture)
      {
        ID3D11Texture2D* pBackBuffer = nullptr;
        DX11_CHECK(_pWindow->m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));

        _pWindow->m_pContext->ResolveSubresource(pBackBuffer, 0u, _pWindow->m_pColorTexture.Get(), 0u, _pWindow->m_eSwapchainFormat);

        pBackBuffer->Release();
      }

      DX11_CHECK(_pWindow->m_pSwapchain->Present(1u, 0u))
    }

    // Misc

    void ImGuiNewFrame()
    {
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();
    }

  }
}

#endif
