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

  uint32_t GetDefaultMsaaSamples()
  {
    return API::GetDefaultMsaaSamples();
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

  bool IsDefaultRenderTargetBound(APIWindow* _pWindow)
  {
    return API::IsDefaultRenderTargetBound(_pWindow);
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

  APITexture* CreateAPITexture(const void* const* _ppData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage, const SamplerConfig& _rSamplerConfig, bool _bIsCubemap)
  {
    return API::CreateAPITexture(_ppData, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage, _rSamplerConfig, _bIsCubemap);
  }

  void BindAPITexture(APITexture* _pTexture)
  {
    API::BindAPITexture(_pTexture);
  }

  void UnbindAPITexture(APITexture* _pTexture)
  {
    API::UnbindAPITexture(_pTexture);
  }

  void ClearAPITexture(APITexture* _pTexture, TextureUsage _eUsage)
  {
    API::ClearAPITexture(_pTexture, _eUsage);
  }
  
  void DestroyAPITexture(APITexture* _pTexture)
  {
    API::DestroyAPITexture(_pTexture);
  }

  // RenderTarget

  APIRenderTarget* CreateAPIRenderTarget()
  {
    return API::CreateAPIRenderTarget();
  }

  void BindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    API::BindAPIRenderTarget(_pRenderTarget);
  }

  void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples)
  {
    API::BeginRenderTargetSetup(_pRenderTarget, _uWidth, _uHeight, _eFormat, _eDepthStencilFormat, _uMsaaSamples);
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

  void EndRenderTargetSetup()
  {
    API::EndRenderTargetSetup();
  }

  void ClearAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    API::ClearAPIRenderTarget(_pRenderTarget);
  }

  void SetUsingAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    API::SetUsingAPIRenderTarget(_pRenderTarget);
  }

  void UnbindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    API::UnbindAPIRenderTarget(_pRenderTarget);
  }

  bool IsAPIRenderTargetBound(APIRenderTarget* _pRenderTarget)
  {
    return API::IsAPIRenderTargetBound(_pRenderTarget);
  }

  void DestroyAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    API::DestroyAPIRenderTarget(_pRenderTarget);
  }


  // Render state

  APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo, uint32_t _uMsaaSamples)
  {
    return API::CreateAPIRenderState(_oInfo, _uMsaaSamples);
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


  // Render sub state

  APIRenderSubState* CreateAPIRenderSubState(ResourceFrequency _eFrequency)
  {
    return API::CreateAPIRenderSubState(_eFrequency);
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

  void EndSubStateSetup(ResourceFrequency _eFrequency)
  {
    API::EndSubStateSetup(_eFrequency);
  }

  void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency)
  {
    API::BindAPIRenderSubState(_pAPIRenderSubState, _eFrequency);
  }

  void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    API::DestroyRenderSubState(_pAPIRenderSubState);
  }

  // Drawing

  void WaitForNextImage(APIWindow* _pWindow)
  {
    API::WaitForNextImage(_pWindow);
  }

  int BeginDraw(APIWindow* _pWindow)
  {
    return API::BeginDraw(_pWindow);
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uVertexCount, const void* _pConstantData, uint32_t _uConstantSize)
  {
    API::DrawMesh(_pMesh, _uVertexCount, _pConstantData, _uConstantSize);
  }

  void EndDraw(APIWindow* _pWindow)
  {
    API::EndDraw(_pWindow);
  }    

  // Misc 

  void ImGuiNewFrame()
  {
    API::ImGuiNewFrame();
  }

}
