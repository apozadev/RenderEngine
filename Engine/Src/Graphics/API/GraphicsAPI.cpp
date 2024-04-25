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

  void BindAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    RENDER_API::BindAPIRenderState(_pAPIRenderState);
  }

  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    RENDER_API::DestroyAPIRenderState(_pAPIRenderState);
  }

  void BeginRenderStateSetup(APIRenderState* _pRenderState)
  {
    RENDER_API::BeginRenderStateSetup(_pRenderState);
  }

  void RenderStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t size)
  {
    RENDER_API::RenderStateSetupConstantBuffer(_pCBuffer, size);
  }

  void RenderStateSetupTexture(APITexture* _pTexture)
  {
    RENDER_API::RenderStateSetupTexture(_pTexture);
  }

  void EndRenderStateSetup()
  {
    RENDER_API::EndRenderStateSetup();
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
