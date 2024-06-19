#pragma once

#include "Graphics/API/DX11/DX11API.h"

#include <d3d11.h>
#include <wrl.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Core/Exception.h"
#include "Graphics/API/DX11/DX11Macros.h"

#include "Graphics/API/DX11/APIWindow.h"

namespace api
{
  namespace dx11
  {
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

      HWND hWnd = glfwGetWin32Window(_pGlfwWindow);

      RECT rect;
      if (!GetWindowRect(hWnd, &rect))
      {
        THROW_GENERIC_EXCEPTION("[DX11] Could not get window rect")
      }

      pWindow->m_uWidth = rect.right - rect.left;
      pWindow->m_uHeight = rect.bottom - rect.top;

      DXGI_SWAP_CHAIN_DESC oSwapchainDesc;
      ZeroMemory(&oSwapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
      oSwapchainDesc.BufferDesc.Width = pWindow->m_uWidth;
      oSwapchainDesc.BufferDesc.Height = pWindow->m_uHeight;
      oSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      oSwapchainDesc.BufferDesc.RefreshRate.Numerator = 0;
      oSwapchainDesc.BufferDesc.RefreshRate.Denominator = 0;
      oSwapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
      oSwapchainDesc.SampleDesc.Count = 1;
      oSwapchainDesc.SampleDesc.Quality = 0;
      oSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      oSwapchainDesc.BufferCount = 2;
      oSwapchainDesc.OutputWindow = hWnd;
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
        &pWindow->m_pSwapchain,
        &pWindow->m_pDevice,
        nullptr,
        &pWindow->m_pContext));



      return pWindow;
    }

    void SetUsingAPIWindow(APIWindow* _pWindow)
    {
    }

    void OnWindowResize(APIWindow* _pWindow)
    {
    }

    uint32_t GetWindowWidth(APIWindow* _pWindow)
    {
      return 0;
    }

    uint32_t GetWindowHeight(APIWindow* _pWindow)
    {
      return 0u;
    }

    void DestroyAPIWindow(APIWindow* _pAPIWindow)
    {
      delete _pAPIWindow;
    }

    // Camera

    APICamera* CreateAPICamera()
    {
      return nullptr;
    }

    void BeginCameraSubStateSetup(APICamera* _pCamera)
    {
    }

    void EndCameraSubstateSetup(APICamera* _pCamera)
    {
    }

    void BindAPICamera(APICamera* _pCamera)
    {
    }

    void DestroyAPICamera(APICamera* _pCamera)
    {
    }

    // Mesh

    APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize)
    {
      return nullptr;
    }

    void DestroyAPIMesh(APIMesh* _pMesh)
    {
    }

    // Constant buffer

    APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
    {
      return nullptr;
    }

    void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize)
    {
    }

    void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
    {
    }

    void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer)
    {
    }

    // Texture

    APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels)
    {
      return nullptr;
    }

    void BindAPITexture(APITexture* _pTexture)
    {
    }

    void DestroyAPITexture(APITexture* _pTexture)
    {
    }

    // Render state

    APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo)
    {
      return nullptr;
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
    }

    void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
    {
    }

    // Render substate

    APIRenderSubState* CreateAPIRenderSubState()
    {
      return nullptr;
    }

    void BeginSubStateSetup(APIRenderSubState* _pAPIRenderState)
    {
    }

    void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo)
    {
    }

    void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
    {
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
      return 0;
    }

    void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount, void* _pConstantData, uint32_t _uConstantSize)
    {
    }

    void EndDraw(APIWindow* _pWindow)
    {

      DX11_CHECK(_pWindow->m_pSwapchain->Present(1u, 0u))

    }

  }
}