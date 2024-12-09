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
#include "Graphics/API/DX11/APIInternal.h"
#include "Graphics/API/DX11/DX11Pools.h"

namespace wrl = Microsoft::WRL;

namespace api
{
  namespace dx11
  {
    
    // General

    void InitializeAPI()
    {
      s_oGlobalData.m_uMaxMsaaSamples = 0u;
      s_oRenderStatePool.Initialize();
      s_oConstantBufferPool.Initialize();
      s_oMeshPool.Initialize();
      s_oTexturePool.Initialize();
      s_oRenderTargetPool.Initialize();
    }

    void ShutDownAPI()
    {
      s_oRenderStatePool.Clear();
      s_oConstantBufferPool.Clear();
      s_oMeshPool.Clear();
      s_oTexturePool.Clear();
      s_oRenderTargetPool.Clear();
    }

    uint32_t GetDefaultMsaaSamples()
    {
      return s_oGlobalData.m_uMaxMsaaSamples;
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
      
      if (s_oGlobalData.m_uMaxMsaaSamples < 1u)
      {
        s_oGlobalData.m_uMaxMsaaSamples = QueryMaxSupportedMsaaSamples(pWindow);
      }

      CreateWindowRenderTarget(pWindow, s_oGlobalData.m_uMaxMsaaSamples);

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
    }

    void UnbindDefaultRenderTarget(APIWindow* _pWindow)
    {
      ID3D11RenderTargetView* pNullView =  nullptr;      
      _pWindow->m_pContext->OMSetRenderTargets(1, &pNullView, nullptr);
    }

    void DestroyAPIWindow(APIWindow* _pAPIWindow)
    {
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

    // Camera

    APICamera* CreateAPICamera()
    {
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;
      APICamera* pCamera = new APICamera();

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
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

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
      s_oMeshPool.ReturnElement(_pMesh);
    }

    // Constant buffer

    APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
    {
      APIConstantBuffer* pCBuffer = s_oConstantBufferPool.PullElement();

      D3D11_BUFFER_DESC oDesc = {};
      oDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      oDesc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;
      oDesc.CPUAccessFlags = 0u;// D3D11_CPU_ACCESS_WRITE;
      oDesc.MiscFlags = 0u;
      oDesc.ByteWidth = _uSize;
      oDesc.StructureByteStride = 0;

      DX11_CHECK(s_oGlobalData.m_pUsingWindow->m_pDevice->CreateBuffer(&oDesc, NULL, pCBuffer->m_pCBuffer.GetAddressOf()));
      
      return pCBuffer;
    }

    void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize)
    {            
      /*D3D11_MAPPED_SUBRESOURCE oMsr;
      DX11_CHECK(pWindow->m_pContext->Map(_pCBuffer->m_pCBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &oMsr));

      memcpy(oMsr.pData, &_pData, _uSize);

      pWindow->m_pContext->Unmap(_pCBuffer->m_pCBuffer.Get(), 0u);*/

      s_oGlobalData.m_pUsingWindow->m_pContext->UpdateSubresource(_pCBuffer->m_pCBuffer.Get(), 0u, nullptr, _pData, 0u, 0u);

    }

    void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
    {

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

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
      s_oConstantBufferPool.ReturnElement(_pCBuffer);
    }

    // Texture

    APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage)
    {
      APITexture* pTexture = s_oTexturePool.PullElement();

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      uint32_t uPixelSize = GetImageFormatSize(_eFormat);

      uint32_t uMemPitch = uPixelSize * _uWidth;

      DXGI_FORMAT eDXGIFormat = GetDXGIFormat(_eFormat);
      DXGI_FORMAT eDXGISrvFormat = GetDXGISrvFormat(_eFormat);

      uint32_t uMaxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_uWidth, _uWidth)))) + 1;
      _uMipLevels = _uMipLevels == 0u ? uMaxMipLevels : std::min(_uMipLevels, uMaxMipLevels);

      uint32_t uBindFlags = GetTextureUsage(_uUsage);
      if (_uMipLevels > 1u)
      {
        uBindFlags |= D3D11_BIND_RENDER_TARGET;
      }

      D3D11_SUBRESOURCE_DATA oData = {};
      oData.pSysMem = _pData;
      oData.SysMemPitch = uMemPitch;

      D3D11_TEXTURE2D_DESC oTexDesc = {};
      oTexDesc.ArraySize = 1;
      oTexDesc.BindFlags = uBindFlags;
      oTexDesc.CPUAccessFlags = 0;
      oTexDesc.Format = eDXGIFormat;
      oTexDesc.SampleDesc.Count = _uMsaaSamples;
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
        DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&oTexDesc, _pData? &oData : nullptr, pTexture->m_pTexture.GetAddressOf()));
      }
      else
      {
        //GFX_THROW_INFO(GetDevice()->CreateTexture2D(&desc, NULL, m_texture.GetAddressOf()));
        DX11_CHECK(pWindow->m_pDevice->CreateTexture2D(&oTexDesc, nullptr, pTexture->m_pTexture.GetAddressOf()));
      }

      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
      srvDesc.Format = eDXGISrvFormat;
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
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

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

    void UnbindAPITexture(APITexture* _pTexture)
    {
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      uint32_t uStageMsk = static_cast<uint32_t>(_pTexture->m_eStage);

      ID3D11ShaderResourceView* pNullSrv = nullptr;

      if ((uStageMsk & static_cast<uint32_t>(PipelineStage::VERTEX)) != 0u)
      {
        pWindow->m_pContext->VSSetShaderResources(_pTexture->m_uSlot, 1u, &pNullSrv);
      }
      if ((uStageMsk & static_cast<uint32_t>(PipelineStage::PIXEL)) != 0u)
      {
        pWindow->m_pContext->PSSetShaderResources(_pTexture->m_uSlot, 1u, &pNullSrv);
      }
    }

    void ClearAPITexture(APITexture* /*_pTexture*/, TextureUsage /*_eUsage*/)
    {

    }

    void DestroyAPITexture(APITexture* _pTexture)
    {
      s_oTexturePool.ReturnElement(_pTexture);
    }

    // RenderTarget

    APIRenderTarget* CreateAPIRenderTarget()
    {
      APIRenderTarget* pRenderTarget = s_oRenderTargetPool.PullElement();

      return pRenderTarget;
    }

    void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples)
    {
      s_oGlobalData.m_oRenderTargetBuilder.SetColorFormat(GetDXGIFormat(_eFormat));
      s_oGlobalData.m_oRenderTargetBuilder.SetDepthStencilFormat(GetDXGIFormat(_eDepthStencilFormat));
      s_oGlobalData.m_oRenderTargetBuilder.SetMsaaSamples(_uMsaaSamples);
      s_oGlobalData.m_pUsingRenderTarget = _pRenderTarget;
    }

    void RenderTargetAddColorTexture(APITexture* _pTexture)
    {            
      s_oGlobalData.m_oRenderTargetBuilder.AddColorTexture(_pTexture);
    }

    void RenderTargetSetDepthStencilTexture(APITexture* _pTexture)
    {            
      s_oGlobalData.m_oRenderTargetBuilder.SetDepthTexture(_pTexture);
    }

    void RenderTargetAddColorResolveTexture(APITexture* _pTexture)
    {
      s_oGlobalData.m_oRenderTargetBuilder.AddResolveColorTexture(_pTexture);
    }

    void EndRenderTargetSetup()
    {
      s_oGlobalData.m_oRenderTargetBuilder.Build(s_oGlobalData.m_pUsingRenderTarget);
    }

    void ClearAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      float aClearColor[] = { 0.f,0.f,0.f,1.f };
      unsigned int uClearFlags = D3D11_CLEAR_DEPTH;

      for (auto& pRtv : _pRenderTarget->m_lstRtv)
      {
        pWindow->m_pContext->ClearRenderTargetView(pRtv.Get(), aClearColor);
      }

      if (_pRenderTarget->m_pDsv.Get())
      {
        pWindow->m_pContext->ClearDepthStencilView(_pRenderTarget->m_pDsv.Get(), uClearFlags, 1.0f, 0u);
      }

    }

    void BindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      
      s_oGlobalData.m_pUsingWindow->m_pContext->OMSetRenderTargets(_pRenderTarget->m_lstRtv.size(), _pRenderTarget->m_lstRtv[0].GetAddressOf(), _pRenderTarget->m_pDsv.Get());
    }

    void SetUsingAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      s_oGlobalData.m_pUsingRenderTarget = _pRenderTarget;
    }

    void UnbindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      const std::vector<ID3D11RenderTargetView*> lstNullViews(_pRenderTarget->m_lstRtv.size(), nullptr);
      
      s_oGlobalData.m_pUsingWindow->m_pContext->OMSetRenderTargets(_pRenderTarget->m_lstRtv.size(), lstNullViews.data(), nullptr);
    }
    
    void DestroyAPIRenderTarget(APIRenderTarget* _pRenderTarget)
    {
      s_oRenderTargetPool.ReturnElement(_pRenderTarget);
    }

    // Render state

    APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo, uint32_t /*_uMsaaSamples*/)
    {

      APIRenderState* pRenderState = s_oRenderStatePool.PullElement();

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

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
      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

      pWindow->m_pContext->VSSetShader(_pAPIRenderState->m_pVertexShader.Get(), nullptr, 0u);
      pWindow->m_pContext->PSSetShader(_pAPIRenderState->m_pPixelShader.Get(), nullptr, 0u);
      pWindow->m_pContext->IASetInputLayout(_pAPIRenderState->m_pInputLayout.Get());
      pWindow->m_pContext->RSSetState(_pAPIRenderState->m_pRasterizer.Get());
      pWindow->m_pContext->OMSetBlendState(_pAPIRenderState->m_pBlendState.Get(), nullptr, 0xffffffff);
      pWindow->m_pContext->OMSetDepthStencilState(_pAPIRenderState->m_pDSState.Get(), 0);
    }

    void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
    {
      s_oRenderStatePool.ReturnElement(_pAPIRenderState);
    }

    // Render substate

    APIRenderSubState* CreateAPIRenderSubState(ResourceFrequency _eFrequency)
    {
      return nullptr;
    }

    void BeginSubStateSetup(APIRenderSubState* _pAPIRenderState)
    {
    }

    void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo)
    {      

      _pCBuffer->m_stageMask = static_cast<unsigned int>(_oBindInfo.m_eStage);

      /*uint32_t uSlotOffset = 0u;

      if ((_pCBuffer->m_stageMask & static_cast<uint32_t>(PipelineStage::VERTEX)) != 0u)
      {
        switch (_oBindInfo.m_eLevel)
        {
        case ResourceFrequency::GLOBAL:
          uSlotOffset = VS_SLOT_OFFSET_GLOBAL;
          break;
        case ResourceFrequency::RENDER_STEP:
          uSlotOffset = VS_SLOT_OFFSET_STEP;
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
        case ResourceFrequency::RENDER_STEP:
          uSlotOffset = PS_SLOT_OFFSET_STEP;
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

      _pCBuffer->m_slot = _oBindInfo.m_iBinding + uSlotOffset;      */

      _pCBuffer->m_slot = static_cast<unsigned int>(/*_oBindInfo.m_iBinding +*/ static_cast<int>(_oBindInfo.m_eLevel) /** 16*/);

    }

    void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
    {
      _pTexture->m_uSlot = static_cast<unsigned int>(_oBindInfo.m_iBinding + static_cast<int>(_oBindInfo.m_eLevel) * 16);
      _pTexture->m_eStage = _oBindInfo.m_eStage;
    }

    void EndSubStateSetup(ResourceFrequency _eFrequency)
    {
    }

    void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency)
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
      
      return 0;
    }

    void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount, const void* _pConstantData, uint32_t _uConstantSize)
    {

      APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

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
      
      _pWindow->m_pContext->ResolveSubresource(pBackBuffer, 0u, _pWindow->m_pColorTexture.Get(), 0u, _pWindow->m_eSwapchainFormat);

      DX11_CHECK(_pWindow->m_pSwapchain->Present(1u, 0u))

    }

  }
}