#include "Graphics/API/GraphicsAPI.h"

#if RENDER_API == vk
#include "Graphics/API/Vulkan/VulkanAPI.h"
#endif

namespace api
{    

  // Global

  void InitializeAPI()
  {
    RENDER_API::InitializeAPI();
  }

  void ShutDownAPI()
  {
    RENDER_API::ShutDownAPI();
  } 

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
  {    
    return RENDER_API::CreateAPIWindow(_pGlfwWindow);
  }

  void SetUsingAPIWindow(APIWindow* _pWindow)
  {
    RENDER_API::SetUsingAPIWindow(_pWindow);
  }

  void OnWindowResize(APIWindow* _pWindow)
  {
    RENDER_API::OnWindowResize(_pWindow);
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {
    RENDER_API::DestroyAPIWindow(_pWindow);
  }

  // Camera

  APICamera* CreateAPICamera()
  {
    return RENDER_API::CreateAPICamera();
  }

  void BeginCameraSubStateSetup(APICamera* _pCamera)
  {
    RENDER_API::BeginCameraSubStateSetup(_pCamera);
  }

  void EndCameraSubstateSetup(APICamera* _pCamera)
  {
    RENDER_API::EndCameraSubstateSetup(_pCamera);
  }

  void BindAPICamera(APICamera* _pCamera)
  {
    RENDER_API::BindAPICamera(_pCamera);
  }

  void DestroyAPICamera(APICamera* _pCamera)
  {
    RENDER_API::DestroyAPICamera(_pCamera);
  }

  // Mesh

  APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize)
  {
    return RENDER_API::CreateAPIMesh(_pVertexData, _uVertexDataSize, _pIndexData, _uIndexDataSize);
  }

  void DestroyAPIMesh(APIMesh* _pMesh)
  {
    RENDER_API::DestroyAPIMesh(_pMesh);
  }

  // ConstantBuffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
  {
    return RENDER_API::CreateAPIConstantBuffer(_uSize);
  }

  void UpdateAPIConstantBuffer(APIConstantBuffer* _pCbuffer, const void* _pData, size_t _uSize)
  {
    RENDER_API::UpdateAPIConstanBuffer(_pCbuffer, _pData, _uSize);
  }

  void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
  {
    RENDER_API::BindAPIConstantBuffer(_pCbuffer);
  }

  void DestroyAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
  {
    RENDER_API::DestroyAPIConstanBuffer(_pCbuffer);
  }

  // Texture

  APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat)
  {
    return RENDER_API::CreateAPITexture(_pData, _uWidth, _uHeight, _eFormat);
  }

  void BindAPITexture(APITexture* _pTexture)
  {
    RENDER_API::BindAPITexture(_pTexture);
  }
  
  void DestroyAPITexture(APITexture* _pTexture)
  {
    RENDER_API::DestroyAPITexture(_pTexture);
  }

  // Render state

  APIRenderState* CreateAPIRenderState()
  {
    return RENDER_API::CreateAPIRenderState();
  }

  void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
  {
    RENDER_API::BeginRenderStateSetup(_pAPIRenderState);
  }

  void EndRenderStateSetup()
  {
    RENDER_API::EndRenderStateSetup();
  }

  void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    RENDER_API::SetUsingAPIRenderState(_pAPIRenderState);
  }

  void BindAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    RENDER_API::BindAPIRenderState(_pAPIRenderState);
  }

  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    RENDER_API::DestroyAPIRenderState(_pAPIRenderState);
  }

  // Render sub state

  APIRenderSubState* CreateAPIRenderSubState()
  {
    return RENDER_API::CreateAPIRenderSubState();
  }

  void BeginSubStateSetup(APIRenderSubState* _pAPIRenderSubState)
  {
    RENDER_API::BeginSubStateSetup(_pAPIRenderSubState);
  }

  void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo)
  {
    RENDER_API::SubStateSetupConstantBuffer(_pCBuffer, size, _oBindInfo);
  }

  void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
  {
    RENDER_API::SubStateSetupTexture(_pTexture, _oBindInfo);
  }

  void EndSubStateSetup()
  {
    RENDER_API::EndSubStateSetup();
  }

  void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    RENDER_API::BindAPIRenderSubState(_pAPIRenderSubState);
  }

  void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    RENDER_API::DestroyRenderSubState(_pAPIRenderSubState);
  }

  // Drawing

  int BeginDraw(APIWindow* _pWindow)
  {
    return RENDER_API::BeginDraw(_pWindow);
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount)
  {
    RENDER_API::DrawMesh(_pMesh, _uVertexCount);
  }

  void EndDraw(APIWindow* _pWindow)
  {
    RENDER_API::EndDraw(_pWindow);
  }    

}
