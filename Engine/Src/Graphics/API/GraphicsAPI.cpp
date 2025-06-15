#include "Graphics/API/GraphicsAPI.h"


#if defined RENDER_API_VULKAN
#include "Graphics/API/Vulkan/VulkanAPI.h"
#elif defined RENDER_API_DX11
#include "Graphics/API/DX11/DX11API.h"
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

  uint32_t GetMaxMsaaSamples()
  {
    return API::GetMaxMsaaSamples();
  }

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow, uint32_t _uMsaaSamples)
  {    
    return API::CreateAPIWindow(_pGlfwWindow, _uMsaaSamples);
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

  uint32_t GetWindowMSAASamples(APIWindow* _pWindow)
  {
    return API::GetWindowMSAASamples(_pWindow);
  }

  void ClearDefaultRenderTarget(APIWindow* _pWindow)
  {
    API::ClearDefaultRenderTarget(_pWindow);
  }

  void BindDefaultRenderTarget(APIWindow* _pWindow)
  {
    API::BindDefaultRenderTarget(_pWindow);
  }

  void UnbindDefaultRenderTarget(APIWindow* _pWindow)
  {
    API::UnbindDefaultRenderTarget(_pWindow);
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {
    API::DestroyAPIWindow(_pWindow);
  }

  // Mesh

  APIMesh* CreateAPIMesh(const APIWindow* _pWindow, const void* _pVertexData, size_t _uVertexDataSize, const void* _pIndexData, size_t _uIndexDataSize)
  {
    return API::CreateAPIMesh(_pWindow, _pVertexData, _uVertexDataSize, _pIndexData, _uIndexDataSize);
  }

  void DestroyAPIMesh(const APIWindow* _pWindow, APIMesh* _pMesh)
  {
    API::DestroyAPIMesh(_pWindow, _pMesh);
  }

  // ConstantBuffer

  APIConstantBuffer* CreateAPIConstantBuffer(const APIWindow* _pWindow, size_t _uSize)
  {
    return API::CreateAPIConstantBuffer(_pWindow, _uSize);
  }

  void UpdateAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCbuffer, const void* _pData, size_t _uSize)
  {
    API::UpdateAPIConstantBuffer(_pWindow, _pCbuffer, _pData, _uSize);
  }

  void BindAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCbuffer)
  {
    API::BindAPIConstantBuffer(_pWindow, _pCbuffer);
  }

  void DestroyAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCbuffer)
  {
    API::DestroyAPIConstantBuffer(_pWindow, _pCbuffer);
  }

  // Texture

  APITexture* CreateAPITexture(const APIWindow* _pWindow, const void* const* _ppData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage, const SamplerConfig& _rSamplerConfig, bool _bIsCubemap)
  {
    return API::CreateAPITexture(_pWindow, _ppData, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage, _rSamplerConfig, _bIsCubemap);
  }

  void GenerateMipMaps(const APIWindow* _pWindow, APITexture* _pTexture)
  {
    API::GenerateMipMaps(_pWindow, _pTexture);
  }

  void BindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
  {
    API::BindAPITexture(_pWindow, _pTexture);
  }

  void UnbindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
  {
    API::UnbindAPITexture(_pWindow, _pTexture);
  }

  void ClearAPITexture(const APIWindow* _pWindow, APITexture* _pTexture, TextureUsage _eUsage)
  {
    API::ClearAPITexture(_pWindow, _pTexture, _eUsage);
  }
  
  void DestroyAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
  {
    API::DestroyAPITexture(_pWindow, _pTexture);
  }

  // RenderTarget

  APIRenderTarget* CreateAPIRenderTarget()
  {
    return API::CreateAPIRenderTarget();
  }

  void BindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {
    API::BindAPIRenderTarget(_pWindow, _pRenderTarget);
  }

  void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples, bool _bIsCubemap)
  {
    API::BeginRenderTargetSetup(_pRenderTarget, _uWidth, _uHeight, _eFormat, _eDepthStencilFormat, _uMsaaSamples, _bIsCubemap);
  }

  void RenderTargetAddColorTexture(APITexture* _pTexture)
  {
    API::RenderTargetAddColorTexture(_pTexture);
  }

  void RenderTargetSetDepthStencilTexture(APITexture* _pTexture)
  {
    API::RenderTargetSetDepthStencilTexture(_pTexture);
  }

  void RenderTargetAddColorResolveTexture(APITexture* _pTexture)
  {
    API::RenderTargetAddColorResolveTexture(_pTexture);
  }

  void EndRenderTargetSetup(const APIWindow* _pWindow)
  {
    API::EndRenderTargetSetup(_pWindow);
  }

  void ClearAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {
    API::ClearAPIRenderTarget(_pWindow, _pRenderTarget);
  }

  void UnbindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {
    API::UnbindAPIRenderTarget(_pWindow, _pRenderTarget);
  }

  void DestroyAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {
    API::DestroyAPIRenderTarget(_pWindow, _pRenderTarget);
  }


  // Render state

  APIRenderState* CreateAPIRenderState(const APIWindow* _pWindow, const RenderStateInfo& _oInfo, APIRenderTarget* _pRenderTarget, uint32_t _uMsaaSamples)
  {
    return API::CreateAPIRenderState(_pWindow, _oInfo, _pRenderTarget, _uMsaaSamples);
  }

  void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
  {
    API::BeginRenderStateSetup(_pAPIRenderState);
  }

  void RenderStateSetupConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState)
  {
    API::RenderStateSetupConstantBuffer(_pWindow, _pCBuffer, size, _oBindInfo, _pRenderState);
  }

  void RenderStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState)
  {
    API::RenderStateSetupTexture(_pWindow, _pTexture, _oBindInfo, _pRenderState);
  }

  void EndRenderStateSetup(const APIWindow* _pWindow)
  {
    API::EndRenderStateSetup(_pWindow);
  }

  void BindAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState)
  {
    API::BindAPIRenderState(_pWindow, _pAPIRenderState);
  }

  void DestroyAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState)
  {
    API::DestroyAPIRenderState(_pWindow, _pAPIRenderState);
  }

  // Shader Reflection

  uint32_t GetConstantBufferCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage)
  {
    return API::GetConstantBufferCount(_pAPIRenderState, _eStage);
  }

  uint32_t GetTextureCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage)
  {
    return API::GetTextureCount(_pAPIRenderState, _eStage);
  }

  std::string GetConstantBufferName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
  {
    return API::GetConstantBufferName(_pAPIRenderState, _eStage, _uIdx);
  }

  const char* GetTextureName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
  {
    return API::GetTextureName(_pAPIRenderState, _eStage, _uIdx);
  }

  uint32_t GetConstantBufferMemberCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
  {
    return API::GetConstantBufferMemberCount(_pAPIRenderState, _eStage, _uIdx);
  }

  std::string GetConstantBufferMemberName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {
    return API::GetConstantBufferMemberName(_pAPIRenderState, _eStage, _uIdx, _uMemberIdx);
  }

  size_t GetConstantBufferMemberSize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {
    return API::GetConstantBufferMemberSize(_pAPIRenderState, _eStage, _uIdx, _uMemberIdx);
  }

  ConstantBufferType GetConstantBufferMemberType(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {
    return API::GetConstantBufferMemberType(_pAPIRenderState, _eStage, _uIdx, _uMemberIdx);
  }

  uint32_t GetConstantBufferMemberArraySize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {
    return API::GetConstantBufferMemberArraySize(_pAPIRenderState, _eStage, _uIdx, _uMemberIdx);
  }


  // Render sub state

  APIRenderSubState* CreateAPIRenderSubState(const APIWindow* _pWindow, ResourceFrequency _eFrequency)
  {
    return API::CreateAPIRenderSubState(_pWindow, _eFrequency);
  }

  void BeginSubStateSetup(const APIWindow* _pWindow, APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency)
  {
    API::BeginSubStateSetup(_pWindow, _pAPIRenderSubState, _eFrequency);
  }

  void SubStateSetupConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t size, const ResourceBindInfo& _oBindInfo)
  {
    API::SubStateSetupConstantBuffer(_pWindow, _pCBuffer, size, _oBindInfo);
  }

  void SubStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
  {
    API::SubStateSetupTexture(_pWindow, _pTexture, _oBindInfo);
  }

  void EndSubStateSetup(const APIWindow* _pWindow)
  {
    API::EndSubStateSetup(_pWindow);
  }

  void BindAPIRenderSubState(const APIWindow* _pWindow, APIRenderState* _pRenderState, APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency)
  {
    API::BindAPIRenderSubState(_pWindow, _pRenderState, _pAPIRenderSubState, _eFrequency);
  }

  void DestroyRenderSubState(const APIWindow* _pWindow, APIRenderSubState* _pAPIRenderSubState)
  {
    API::DestroyRenderSubState(_pWindow, _pAPIRenderSubState);
  }

  // Drawing

  void WaitForLastImage(const APIWindow* _pWindow)
  {
    API::WaitForLastImage(_pWindow);
  }

  int BeginDraw(APIWindow* _pWindow)
  {
    return API::BeginDraw(_pWindow);
  }

  void BeginDrawOffline(APIWindow* _pWindow)
  {
    return API::BeginDrawOffline(_pWindow);
  }

  void DrawMesh(const APIWindow* _pWindow, APIRenderState* _pRenderState, APIMesh* _pMesh, uint32_t _uIndexCount, const void* _pConstantData, uint32_t _uConstantSize)
  {
    API::DrawMesh(_pWindow, _pRenderState, _pMesh, _uIndexCount, _pConstantData, _uConstantSize);
  }

  void EndDraw(APIWindow* _pWindow)
  {
    API::EndDraw(_pWindow);
  }    

  void Present(APIWindow* _pWindow)
  {
    API::Present(_pWindow);
  }

  // Misc 

  void ImGuiNewFrame()
  {
    API::ImGuiNewFrame();
  }

}
