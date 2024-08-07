#include "Graphics/API/GraphicsAPI.h"

#if RENDER_API == 0
#include "Graphics/API/Vulkan/VulkanAPI.h"
#elif RENDER_API == 1
#include "Graphics/API/DX11/DX11API.h"
#else
#error "Unknown RENDER_API specified"
#endif

namespace api
{    

  // Global

  void InitializeAPI()
  {
    API::InitializeAPI();
  }

  void ShutDownAPI()
  {
    API::ShutDownAPI();
  } 

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
  {    
    return API::CreateAPIWindow(_pGlfwWindow);
  }

  void SetUsingAPIWindow(APIWindow* _pWindow)
  {
    API::SetUsingAPIWindow(_pWindow);
  }

  void OnWindowResize(APIWindow* _pWindow)
  {
    API::OnWindowResize(_pWindow);
  }

  uint32_t GetWindowWidth(APIWindow* _pWindow)
  {
    return API::GetWindowWidth(_pWindow);
  }

  uint32_t GetWindowHeight(APIWindow* _pWindow)
  {
    return API::GetWindowHeight(_pWindow);
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {
    API::DestroyAPIWindow(_pWindow);
  }

  // Camera

  APICamera* CreateAPICamera()
  {
    return API::CreateAPICamera();
  }

  void BeginCameraSubStateSetup(APICamera* _pCamera)
  {
    API::BeginCameraSubStateSetup(_pCamera);
  }

  void EndCameraSubstateSetup(APICamera* _pCamera)
  {
    API::EndCameraSubstateSetup(_pCamera);
  }

  void BindAPICamera(APICamera* _pCamera)
  {
    API::BindAPICamera(_pCamera);
  }

  void DestroyAPICamera(APICamera* _pCamera)
  {
    API::DestroyAPICamera(_pCamera);
  }

  // Mesh

  APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize)
  {
    return API::CreateAPIMesh(_pVertexData, _uVertexDataSize, _pIndexData, _uIndexDataSize);
  }

  void DestroyAPIMesh(APIMesh* _pMesh)
  {
    API::DestroyAPIMesh(_pMesh);
  }

  // ConstantBuffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
  {
    return API::CreateAPIConstantBuffer(_uSize);
  }

  void UpdateAPIConstantBuffer(APIConstantBuffer* _pCbuffer, const void* _pData, size_t _uSize)
  {
    API::UpdateAPIConstanBuffer(_pCbuffer, _pData, _uSize);
  }

  void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
  {
    API::BindAPIConstantBuffer(_pCbuffer);
  }

  void DestroyAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
  {
    API::DestroyAPIConstanBuffer(_pCbuffer);
  }

  // Texture

  APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels)
  {
    return API::CreateAPITexture(_pData, _uWidth, _uHeight, _eFormat, _uMipLevels);
  }

  void BindAPITexture(APITexture* _pTexture)
  {
    API::BindAPITexture(_pTexture);
  }
  
  void DestroyAPITexture(APITexture* _pTexture)
  {
    API::DestroyAPITexture(_pTexture);
  }

  // Render state

  APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo)
  {
    return API::CreateAPIRenderState(_oInfo);
  }

  void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
  {
    API::BeginRenderStateSetup(_pAPIRenderState);
  }

  void EndRenderStateSetup()
  {
    API::EndRenderStateSetup();
  }

  void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    API::SetUsingAPIRenderState(_pAPIRenderState);
  }

  void BindAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    API::BindAPIRenderState(_pAPIRenderState);
  }

  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    API::DestroyAPIRenderState(_pAPIRenderState);
  }

  // Render sub state

  APIRenderSubState* CreateAPIRenderSubState()
  {
    return API::CreateAPIRenderSubState();
  }

  void BeginSubStateSetup(APIRenderSubState* _pAPIRenderSubState)
  {
    API::BeginSubStateSetup(_pAPIRenderSubState);
  }

  void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo)
  {
    API::SubStateSetupConstantBuffer(_pCBuffer, size, _oBindInfo);
  }

  void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
  {
    API::SubStateSetupTexture(_pTexture, _oBindInfo);
  }

  void EndSubStateSetup()
  {
    API::EndSubStateSetup();
  }

  void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    API::BindAPIRenderSubState(_pAPIRenderSubState);
  }

  void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    API::DestroyRenderSubState(_pAPIRenderSubState);
  }

  // Drawing

  int BeginDraw(APIWindow* _pWindow)
  {
    return API::BeginDraw(_pWindow);
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount, void* _pConstantData, uint32_t _uConstantSize)
  {
    API::DrawMesh(_pMesh, _uVertexCount, _pConstantData, _uConstantSize);
  }

  void EndDraw(APIWindow* _pWindow)
  {
    API::EndDraw(_pWindow);
  }    

}
