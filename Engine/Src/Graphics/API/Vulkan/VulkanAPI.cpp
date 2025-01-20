
#ifdef RENDER_API_VULKAN

#include "Graphics/API/Vulkan/VulkanAPI.h"

#include <vector>
#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../3rd/spirv-reflect/spirv_reflect.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "Core/Exception.h"

#include "Graphics/Window.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/API/Vulkan/APIInternal.h"
#include "Graphics/API/Vulkan/VulkanMacros.h"
#include "Graphics/API/Vulkan/VulkanShaderReflection.h"
#include "Graphics/API/Vulkan/VulkanPools.h"
#include "File/File.h"

namespace api
{
namespace vk
{
  // General

  void InitializeAPI()
  {

    s_oRenderStatePool.Initialize();
    s_oRenderSubStatePool.Initialize();
    s_oConstantBufferPool.Initialize();
    s_oMeshPool.Initialize();
    s_oTexturePool.Initialize();
    s_oRenderTargetPool.Initialize();

    s_oVkBufferPool.Initialize();
    s_oVkDeviceMemoryPool.Initialize();
    s_oVoidPtrPool.Initialize();
    s_oVkDescriptorSetPool.Initialize();
    s_oVkAttachmentReferencePool.Initialize();
    s_oVkAttachmentDescriptionPool.Initialize();

    CreateInstance();
    CreatePhysicalDevice();    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls    
  }

  void ShutDownAPI()
  {        

    s_oRenderStatePool.Clear();
    s_oRenderSubStatePool.Clear();
    s_oConstantBufferPool.Clear();
    s_oMeshPool.Clear();
    s_oTexturePool.Clear();
    s_oRenderTargetPool.Clear();

    s_oVkBufferPool.Clear();
    s_oVkDeviceMemoryPool.Clear();
    s_oVoidPtrPool.Clear();
    s_oVkDescriptorSetPool.Clear();
    s_oVkAttachmentReferencePool.Clear();
    s_oVkAttachmentDescriptionPool.Clear();

    vkDestroyInstance(s_oGlobalData.m_hInstance, NULL);
  }

  uint32_t GetDefaultMsaaSamples()
  {
    return static_cast<uint32_t>(s_oGlobalData.m_eMaxMsaaSampleCount);
  }

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
  {
    APIWindow* pWindow = new APIWindow();

    pWindow->m_pGlfwWindow = _pGlfwWindow;

    CreateLogicalDevice(pWindow);
    RetrieveQueues(pWindow);
    CreateSurface(pWindow, _pGlfwWindow);
    CreateCommandBuffers(pWindow);
    CreateSwapchain(pWindow);
    CreateColorBuffer(pWindow);
    CreateDepthBuffer(pWindow);          
    CreateSyncObjects(pWindow);  
    CreateDescriptorPool(pWindow);
    CreateGlobalDescriptorLayout(pWindow);

    CreateRenderPass(pWindow, 1u, pWindow->m_eSwapchainFormat, true, VK_FORMAT_D32_SFLOAT, s_oGlobalData.m_eMaxMsaaSampleCount, pWindow->m_hRenderPass, false);

    pWindow->m_pFramebuffers = new VkFramebuffer[pWindow->m_uSwapchainImageCount];

    for (int i = 0; i < pWindow->m_uSwapchainImageCount; i++)
    {
      CreateFramebuffer(pWindow, pWindow->m_hRenderPass, pWindow->m_hColorImageView, pWindow->m_hDepthImageView, pWindow->m_pSwapChainImageViews[i], pWindow->m_pFramebuffers[i]);
    }    

    if (s_oGlobalData.m_pUsingWindow == NULL)
    {
      s_oGlobalData.m_pUsingWindow = pWindow;
    }

    pWindow->m_uCurrFrameIdx = 0u;

    // ImGui stuff

    SetupImGui(pWindow);

    return pWindow;

  }  

  void SetUsingAPIWindow(APIWindow* _pWindow)
  {
    s_oGlobalData.m_pUsingWindow = _pWindow;
  }

  void OnWindowResize(APIWindow* _pWindow)
  {
    _pWindow->m_bResized = true;
  }

  uint32_t GetWindowWidth(APIWindow* _pWindow)
  {
    return _pWindow->m_oExtent.width;
  }

  uint32_t GetWindowHeight(APIWindow* _pWindow)
  {
    return _pWindow->m_oExtent.height;
  }

  void ClearDefaultRenderTarget(APIWindow* _pWindow)
  {
    uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    // Depth buffer

    TransitionImageLayout(_pWindow, _pWindow->m_hDepthImage, VK_FORMAT_D32_SFLOAT, VK_REMAINING_MIP_LEVELS, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageSubresourceRange oRange = {};
    oRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    oRange.baseMipLevel = 0;
    oRange.levelCount = VK_REMAINING_MIP_LEVELS;
    oRange.baseArrayLayer = 0;
    oRange.layerCount = 1;
    
    VkClearDepthStencilValue oDepthClearColor = { 1.f, 0.f };
    vkCmdClearDepthStencilImage(_pWindow->m_pCmdBuffers[uFrameIdx], _pWindow->m_hDepthImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oDepthClearColor, 1u, &oRange);    

    TransitionImageLayout(_pWindow, _pWindow->m_hDepthImage, VK_FORMAT_D32_SFLOAT, VK_REMAINING_MIP_LEVELS, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // Color buffer

    TransitionImageLayout(_pWindow, _pWindow->m_hColorImage, _pWindow->m_eSwapchainFormat, VK_REMAINING_MIP_LEVELS, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    oRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    oRange.baseMipLevel = 0;
    oRange.levelCount = VK_REMAINING_MIP_LEVELS;
    oRange.baseArrayLayer = 0;
    oRange.layerCount = 1;
  
    VkClearColorValue oClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    vkCmdClearColorImage(_pWindow->m_pCmdBuffers[uFrameIdx], _pWindow->m_hColorImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oClearColor, 1u, &oRange);

    TransitionImageLayout(_pWindow, _pWindow->m_hColorImage, _pWindow->m_eSwapchainFormat, VK_REMAINING_MIP_LEVELS, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  }

  void BindDefaultRenderTarget(APIWindow* _pWindow)
  {    
    VkClearValue aClearColors[3];
    aClearColors[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };    
    aClearColors[1].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    aClearColors[2].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo oRenderPassBeginInfo{};
    oRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    oRenderPassBeginInfo.renderPass = _pWindow->m_hRenderPass;
    oRenderPassBeginInfo.framebuffer = _pWindow->m_pFramebuffers[_pWindow->m_uCurrSwapchainImageIdx];
    oRenderPassBeginInfo.renderArea.extent = _pWindow->m_oExtent;
    oRenderPassBeginInfo.renderArea.offset = { 0,0 };
    oRenderPassBeginInfo.clearValueCount = 3u;
    oRenderPassBeginInfo.pClearValues = aClearColors;

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    vkCmdBeginRenderPass(_pWindow->m_pCmdBuffers[uFrameIdx], &oRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);    

    _pWindow->m_bDefaultRenderTargetBound = true;
  }

  void UnbindDefaultRenderTarget(APIWindow* _pWindow)
  {
    
    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    // Record dear imgui primitives into command buffer
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _pWindow->m_pCmdBuffers[uFrameIdx]);

    vkCmdEndRenderPass(_pWindow->m_pCmdBuffers[uFrameIdx]);    

    _pWindow->m_bDefaultRenderTargetBound = false;
  }

  bool IsDefaultRenderTargetBound(APIWindow* _pWindow)
  {
    return _pWindow->m_bDefaultRenderTargetBound;
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    vkDestroyRenderPass(_pWindow->m_hDevice, _pWindow->m_hRenderPass, NULL);

    DestroySwapchain(_pWindow);

    delete[] _pWindow->m_pFramebuffers;

    for (int i = 0; i < uNumImages; i++)
    {
      vkDestroySemaphore(_pWindow->m_hDevice, _pWindow->m_pImageAvailableSemaphores[i], NULL);
      vkDestroySemaphore(_pWindow->m_hDevice, _pWindow->m_pRenderFinishedSemaphores[i], NULL);
      vkDestroyFence(_pWindow->m_hDevice, _pWindow->m_pInFlightFences[i], NULL);
    }

    delete[] _pWindow->m_pImageAvailableSemaphores;
    delete[] _pWindow->m_pRenderFinishedSemaphores;
    delete[] _pWindow->m_pInFlightFences;
        
    vkDestroyDescriptorSetLayout(_pWindow->m_hDevice, _pWindow->m_hGlobalDescSetLayout, NULL);        
    vkDestroyDescriptorSetLayout(_pWindow->m_hDevice, _pWindow->m_hStepDescSetLayout, NULL);        
    vkDestroyDescriptorSetLayout(_pWindow->m_hDevice, _pWindow->m_hMatInstanceDescSetLayout, NULL);        

    vkDestroyDescriptorPool(_pWindow->m_hDevice, _pWindow->m_hDescPool, NULL);        

    vkDestroyCommandPool(_pWindow->m_hDevice, _pWindow->m_hCmdPool, NULL);

    delete[] _pWindow->m_pCmdBuffers;    

    vkDestroySurfaceKHR(s_oGlobalData.m_hInstance, _pWindow->m_hSurface, NULL);    

    vkDestroyDevice(_pWindow->m_hDevice, NULL);   

    delete _pWindow;

  }

  // Camera

  APICamera* CreateAPICamera()
  {
    APICamera* pCamera = new APICamera();    

    pCamera->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

    return pCamera;
  }

  void BindAPICamera(APICamera* _pCamera)
  {        
  }

  void DestroyAPICamera(APICamera* _pCamera)
  {
    delete _pCamera;
  }

  // Mesh

  APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize)
  {
    APIMesh* pMesh = s_oMeshPool.PullElement();

    pMesh->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;    

    // Create vertex buffer
    {
      VkBuffer hStagingBuffer;
      VkDeviceMemory hStagingBufferMemory;

      VkMemoryPropertyFlags uProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      VkBufferUsageFlags uUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

      CreateBuffer(pMesh->m_pOwnerWindow, _uVertexDataSize, uUsage, uProperties, hStagingBuffer, hStagingBufferMemory);
      SetBufferData(pMesh->m_pOwnerWindow, _pVertexData, _uVertexDataSize, hStagingBufferMemory);

      uProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      uUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

      CreateBuffer(pMesh->m_pOwnerWindow, _uVertexDataSize, uUsage, uProperties, pMesh->m_hVertexBuffer, pMesh->m_hVertexBufferMemory);

      CopyBuffer(pMesh->m_pOwnerWindow, hStagingBuffer, pMesh->m_hVertexBuffer, _uVertexDataSize);

      vkDestroyBuffer(pMesh->m_pOwnerWindow->m_hDevice, hStagingBuffer, NULL);
      vkFreeMemory(pMesh->m_pOwnerWindow->m_hDevice, hStagingBufferMemory, NULL);
    }

    // Create index buffer
    {
      VkBuffer hStagingBuffer;
      VkDeviceMemory hStagingBufferMemory;

      VkMemoryPropertyFlags uProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      VkBufferUsageFlags uUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

      CreateBuffer(pMesh->m_pOwnerWindow, _uIndexDataSize, uUsage, uProperties, hStagingBuffer, hStagingBufferMemory);
      SetBufferData(pMesh->m_pOwnerWindow, _pIndexData, _uIndexDataSize, hStagingBufferMemory);

      uProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      uUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

      CreateBuffer(pMesh->m_pOwnerWindow, _uIndexDataSize, uUsage, uProperties, pMesh->m_hIndexBuffer, pMesh->m_hIndexBufferMemory);

      CopyBuffer(pMesh->m_pOwnerWindow, hStagingBuffer, pMesh->m_hIndexBuffer, _uIndexDataSize);

      vkDestroyBuffer(pMesh->m_pOwnerWindow->m_hDevice, hStagingBuffer, NULL);
      vkFreeMemory(pMesh->m_pOwnerWindow->m_hDevice, hStagingBufferMemory, NULL);
    }

    return pMesh;
  }

  void DestroyAPIMesh(APIMesh* _pMesh)
  {

    APIWindow* pWindow = _pMesh->m_pOwnerWindow;

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, uNumImages, pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    DestroyBuffer(_pMesh->m_pOwnerWindow, _pMesh->m_hVertexBuffer, _pMesh->m_hVertexBufferMemory);
    DestroyBuffer(_pMesh->m_pOwnerWindow, _pMesh->m_hIndexBuffer, _pMesh->m_hIndexBufferMemory);

    s_oMeshPool.ReturnElement(_pMesh);
  }

  // Constant buffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
  {
    APIConstantBuffer* pCBuffer = s_oConstantBufferPool.PullElement();

    pCBuffer->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

    APIWindow* pWindow = pCBuffer->m_pOwnerWindow;    
    
    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    // Create buffers

    pCBuffer->m_pUniformBuffers = s_oVkBufferPool.PullElements(uNumImages);
    pCBuffer->m_pUniformBuffersMemory = s_oVkDeviceMemoryPool.PullElements(uNumImages);
    pCBuffer->m_pUniformBuffersMapped = s_oVoidPtrPool.PullElements(uNumImages);

    for (uint32_t i = 0; i < uNumImages; i++)
    {
      CreateBuffer(pWindow, _uSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        pCBuffer->m_pUniformBuffers[i],
        pCBuffer->m_pUniformBuffersMemory[i]);

      VK_CHECK(vkMapMemory(pWindow->m_hDevice, pCBuffer->m_pUniformBuffersMemory[i], 0u, _uSize, 0u, &pCBuffer->m_pUniformBuffersMapped[i]))
    }

    return pCBuffer;
  }

  void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize)
  {
    APIWindow* pWindow = _pCBuffer->m_pOwnerWindow;    

    memcpy(_pCBuffer->m_pUniformBuffersMapped[pWindow->m_uCurrFrameIdx], _pData, _uSize);
  }

  void BindAPIConstantBuffer(APIConstantBuffer* /*_pCbuffer*/)
  {
    
  }

  void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer)
  {

    APIWindow* pWindow = _pCBuffer->m_pOwnerWindow;    

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, uNumImages, pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    VkDevice& hDevice = pWindow->m_hDevice;

    for (uint32_t i = 0; i < uNumImages; i++)
    {
      vkUnmapMemory(hDevice, _pCBuffer->m_pUniformBuffersMemory[i]);
      DestroyBuffer(pWindow, _pCBuffer->m_pUniformBuffers[i], _pCBuffer->m_pUniformBuffersMemory[i]);
    }

    s_oVkBufferPool.ReturnElements(_pCBuffer->m_pUniformBuffers);
    s_oVkDeviceMemoryPool.ReturnElements(_pCBuffer->m_pUniformBuffersMemory);
    s_oVoidPtrPool.ReturnElements(_pCBuffer->m_pUniformBuffersMapped);

    s_oConstantBufferPool.ReturnElement(_pCBuffer);
  }

  // Texture

  APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage)
  {

    APITexture* pTexture = s_oTexturePool.PullElement();

    pTexture->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

    APIWindow* pWindow = pTexture->m_pOwnerWindow;

    VkBuffer hStagingBuffer;
    VkDeviceMemory hStagingBufferMemory;       

    size_t uSize = _uWidth* _uHeight* GetImageFormatSize(_eFormat);

    pTexture->m_eFormat = GetVKFormat(_eFormat);

    uint32_t uMaxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_uWidth, _uWidth)))) + 1;
    _uMipLevels = _uMipLevels == 0u? uMaxMipLevels : std::min(_uMipLevels, uMaxMipLevels);

    if (_pData != nullptr)
    {
      CreateBuffer(pWindow
        , uSize
        , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        , hStagingBuffer
        , hStagingBufferMemory);

      SetBufferData(pWindow, _pData, uSize, hStagingBufferMemory);
    }

    VkImageUsageFlags uUsageFlags = GetVkTextureUsage(_uUsage);

    if (_pData != nullptr)
    {
      uUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if (_uMipLevels > 1u)
    {
      uUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    CreateImage(pWindow
      , _uWidth
      , _uHeight
      , pTexture->m_eFormat
      , _uMipLevels
      , static_cast<VkSampleCountFlagBits>(_uMsaaSamples)
      , VK_IMAGE_TILING_OPTIMAL
      , uUsageFlags
      , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
      , pTexture->m_hImage
      , pTexture->m_hMemory);

    uint32_t uAspectFlags = 0u;

    if ((_uUsage & TextureUsage::DEPTH_TARGET) != 0u)
    {
      uAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else
    {
      uAspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (_pData != nullptr)
    {
      TransitionImageLayoutOffline(pWindow
        , pTexture->m_hImage
        , pTexture->m_eFormat
        , _uMipLevels
        , uAspectFlags
        , VK_IMAGE_LAYOUT_UNDEFINED
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      CopyBufferToImage(pWindow, hStagingBuffer, pTexture->m_hImage, _uWidth, _uHeight);

      if (_uMipLevels > 1u)
      {
        GenerateMipmaps(pWindow, pTexture->m_hImage, _uWidth, _uHeight, _uMipLevels);
      }

      //TransitionImageLayoutOffline(pWindow, pTexture->m_hImage, eVkFormat, _uMipLevels, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

      DestroyBuffer(pWindow, hStagingBuffer, hStagingBufferMemory);

    }

    CreateImageView(pWindow
      , pTexture->m_hImage
      , pTexture->m_eFormat
      , _uMipLevels
      , uAspectFlags
      , pTexture->m_hImageView);

    if ((_uUsage & TextureUsage::SHADER_RESOURCE) != 0u)
    {
      CreateTextureSampler(pWindow, pTexture, _uMipLevels);
    }

    return pTexture;
  }

  void BindAPITexture(APITexture* /*_pTexture*/)
  {
    
  }

  void UnbindAPITexture(APITexture* /*_pTexture*/)
  {

  }

  void ClearAPITexture(APITexture* _pTexture, TextureUsage _eUsage)
  {
    APIWindow* pWindow = _pTexture->m_pOwnerWindow;

    uint32_t uFrameIdx = pWindow->m_uCurrFrameIdx;

    uint32_t uAspectFlags = 0u;

    switch (_eUsage)
    {
    case TextureUsage::DEPTH_TARGET:
      uAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
      break;
    case TextureUsage::COLOR_TARGET:
      uAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
      break;
    default:
      THROW_GENERIC_EXCEPTION("[API] [VK] Invalid usage flag in ClearAPITexture")
      break;
    }

    TransitionImageLayout(pWindow, _pTexture->m_hImage, _pTexture->m_eFormat, VK_REMAINING_MIP_LEVELS, uAspectFlags, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageSubresourceRange oRange = {};
    oRange.aspectMask = uAspectFlags;
    oRange.baseMipLevel = 0;
    oRange.levelCount = VK_REMAINING_MIP_LEVELS;
    oRange.baseArrayLayer = 0;
    oRange.layerCount = 1;

    if (_eUsage == TextureUsage::DEPTH_TARGET)
    {
      VkClearDepthStencilValue oClearColor = { 1.f, 0.f };
      vkCmdClearDepthStencilImage(pWindow->m_pCmdBuffers[uFrameIdx], _pTexture->m_hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oClearColor, 1u, &oRange);
    }
    else
    {
      VkClearColorValue oClearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; 
      vkCmdClearColorImage(pWindow->m_pCmdBuffers[uFrameIdx], _pTexture->m_hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oClearColor, 1u, &oRange);
    }

    VkImageLayout eFinalLayout = _eUsage == TextureUsage::COLOR_TARGET ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    TransitionImageLayout(pWindow, _pTexture->m_hImage, _pTexture->m_eFormat, VK_REMAINING_MIP_LEVELS, uAspectFlags, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, eFinalLayout);
  }

  void DestroyAPITexture(APITexture* _pTexture)
  {
    APIWindow* pWindow = _pTexture->m_pOwnerWindow;

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, uNumImages, pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    vkDestroySampler(pWindow->m_hDevice, _pTexture->m_hSampler, NULL);
    vkDestroyImageView(pWindow->m_hDevice, _pTexture->m_hImageView, NULL);
    vkDestroyImage(pWindow->m_hDevice, _pTexture->m_hImage, NULL);
    vkFreeMemory(pWindow->m_hDevice, _pTexture->m_hMemory, NULL);

    s_oTexturePool.ReturnElement(_pTexture);
  }

  // RenderTarget

  APIRenderTarget* CreateAPIRenderTarget()
  {
    APIRenderTarget* pRenderTarget = s_oRenderTargetPool.PullElement();

    pRenderTarget->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;    

    return pRenderTarget;
  }

  void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples)
  {
    s_oGlobalData.m_pUsingRenderTarget = _pRenderTarget;    
    s_oGlobalData.m_oRenderTargetBuilder.m_eColorFormat = GetVKFormat(_eFormat);
    s_oGlobalData.m_oRenderTargetBuilder.m_eDepthStencilFormat = GetVKFormat(_eDepthStencilFormat);
    s_oGlobalData.m_oRenderTargetBuilder.m_uMsaaSamples = _uMsaaSamples;
    s_oGlobalData.m_oRenderTargetBuilder.m_uWidth = _uWidth;
    s_oGlobalData.m_oRenderTargetBuilder.m_uHeight = _uHeight;
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

    s_oGlobalData.m_oRenderTargetBuilder.Clear();
  }

  void ClearAPIRenderTarget(APIRenderTarget* /*_pRenderTarget*/)
  {  
  }

  void SetUsingAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    s_oGlobalData.m_pUsingRenderTarget = _pRenderTarget;
  }

  void BindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    APIWindow* pWindow = _pRenderTarget->m_pOwnerWindow;

    /*VkClearValue aClearColors[3];
    aClearColors[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    aClearColors[1].depthStencil = { 1.0f, 0 };
    aClearColors[2].color = { {0.0f, 0.0f, 0.0f, 1.0f} };*/

    VkExtent2D oExtent = {};
    oExtent.width = _pRenderTarget->m_uWidth;
    oExtent.height = _pRenderTarget->m_uHeight;

    VkRenderPassBeginInfo oRenderPassBeginInfo{};
    oRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    oRenderPassBeginInfo.renderPass = _pRenderTarget->m_hRenderPass;
    oRenderPassBeginInfo.framebuffer = _pRenderTarget->m_hFrameBuffer;
    oRenderPassBeginInfo.renderArea.extent = oExtent;
    oRenderPassBeginInfo.renderArea.offset = { 0,0 };
    oRenderPassBeginInfo.clearValueCount = 0u;//3u;
    oRenderPassBeginInfo.pClearValues = nullptr;//aClearColors;

    const uint32_t uFrameIdx = pWindow->m_uCurrFrameIdx;

    vkCmdBeginRenderPass(pWindow->m_pCmdBuffers[uFrameIdx], &oRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    s_oGlobalData.m_pBoundRenderTarget = _pRenderTarget;
  }

  void UnbindAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    APIWindow* pWindow = _pRenderTarget->m_pOwnerWindow;

    const uint32_t uFrameIdx = pWindow->m_uCurrFrameIdx;

    vkCmdEndRenderPass(pWindow->m_pCmdBuffers[uFrameIdx]);

    s_oGlobalData.m_pBoundRenderTarget = nullptr;
  }

  bool IsAPIRenderTargetBound(APIRenderTarget* _pRenderTarget)
  {
    return s_oGlobalData.m_pBoundRenderTarget == _pRenderTarget;
  }

  void DestroyAPIRenderTarget(APIRenderTarget* _pRenderTarget)
  {
    APIWindow* pWindow = _pRenderTarget->m_pOwnerWindow;

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, uNumImages, pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    vkDestroyFramebuffer(pWindow->m_hDevice, _pRenderTarget->m_hFrameBuffer, nullptr);

    vkDestroyRenderPass(pWindow->m_hDevice, _pRenderTarget->m_hRenderPass, nullptr);

    s_oRenderTargetPool.ReturnElement(_pRenderTarget);
  }

  // Render state

  APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo, uint32_t _uMsaaSamples)
  {

    VkRenderPass hRenderPass = VK_NULL_HANDLE;

    if (s_oGlobalData.m_pUsingRenderTarget)
    {
      hRenderPass = s_oGlobalData.m_pUsingRenderTarget->m_hRenderPass;
    }    
    else
    {
      hRenderPass = s_oGlobalData.m_pUsingWindow->m_hRenderPass;
    }

    APIRenderState* pRenderState = s_oRenderStatePool.PullElement();
    pRenderState->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;  
    uint32_t uNumImages = pRenderState->m_pOwnerWindow->m_uSwapchainImageCount;    

    std::string sVSFilename = _oInfo.m_sVSFilename.substr(0, _oInfo.m_sVSFilename.find_last_of('.')) + ".spv";
    std::string sPSFilename = _oInfo.m_sPSFilename.substr(0, _oInfo.m_sPSFilename.find_last_of('.')) + ".spv";    

    file::File oVSFile(sVSFilename.c_str());
    file::File oPSFile(sPSFilename.c_str());

    /*ReflectSetLayouts(oVSFile, pRenderState->m_oMaterialLayoutBuilder);
    ReflectSetLayouts(oPSFile, pRenderState->m_oMaterialLayoutBuilder);*/

    SpvReflectResult eResult = spvReflectCreateShaderModule(oVSFile.GetSize(), static_cast<void*>(oVSFile.GetData()), &(pRenderState->m_oVertexReflection));
    if (eResult != SPV_REFLECT_RESULT_SUCCESS)
    {
      THROW_GENERIC_EXCEPTION("[API] Error: Unable to load vertex shader reflection")
    }

    eResult = spvReflectCreateShaderModule(oPSFile.GetSize(), static_cast<void*>(oPSFile.GetData()), &(pRenderState->m_oPixelReflection));
    if (eResult != SPV_REFLECT_RESULT_SUCCESS)
    {
      THROW_GENERIC_EXCEPTION("[API] Error: Unable to load pixel shader reflection")
    }
    
    ReflectSetLayouts(pRenderState->m_oVertexReflection, pRenderState->m_oMaterialLayoutBuilder);
    ReflectSetLayouts(pRenderState->m_oPixelReflection, pRenderState->m_oMaterialLayoutBuilder);

    VkDevice hDevice = pRenderState->m_pOwnerWindow->m_hDevice;
    
    pRenderState->m_hDescSetLayout = pRenderState->m_oMaterialLayoutBuilder.Build(hDevice);    

    std::vector<VkDescriptorSetLayout> lstLayouts;
    for (int i = 0; i < uNumImages; i++)
    {
      lstLayouts.push_back(pRenderState->m_hDescSetLayout);
    }

    // Allocate material substate
    VkDescriptorSetAllocateInfo oDescSetAllocInfo {};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = pRenderState->m_pOwnerWindow->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = lstLayouts.data();

    pRenderState->m_pDescSets = s_oVkDescriptorSetPool.PullElements(uNumImages);

    VK_CHECK(vkAllocateDescriptorSets(hDevice, &oDescSetAllocInfo, pRenderState->m_pDescSets))

    CreatePipeline(oVSFile, oPSFile, _oInfo, hRenderPass, static_cast<VkSampleCountFlagBits>(_uMsaaSamples), pRenderState);

    return pRenderState;
  }

  void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
  {

    if (s_oGlobalData.m_pUsingRenderState != nullptr)
    {
      THROW_GENERIC_EXCEPTION("[API] EndRenderStateSetup was not called before a new BeginRenderStateSetup call")
    }

    s_oGlobalData.m_pUsingRenderState = _pAPIRenderState;
    s_oGlobalData.m_pUsingWindow = _pAPIRenderState->m_pOwnerWindow;
  }

  void EndRenderStateSetup()
  {
    APIRenderState* pRenderState = s_oGlobalData.m_pUsingRenderState;
    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;    

    s_oGlobalData.m_oDescSetUpdater.Update(pWindow->m_hDevice, pRenderState->m_pDescSets, uNumImages, &pRenderState->m_oMaterialLayoutBuilder);
    s_oGlobalData.m_oDescSetUpdater.Clear();

    s_oGlobalData.m_pUsingRenderState = nullptr;
  }

  void SetUsingAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    s_oGlobalData.m_pUsingRenderState = _pAPIRenderState;
  }

  void BindAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    APIWindow* pWindow = _pAPIRenderState->m_pOwnerWindow;

    const uint32_t uFrameIdx = pWindow->m_uCurrFrameIdx;

    vkCmdBindPipeline(pWindow->m_pCmdBuffers[uFrameIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hGraphicsPipeline);

    VkViewport oViewport{};
    oViewport.x = 0.0f;
    oViewport.y = (float)pWindow->m_oExtent.height;
    oViewport.width = (float)pWindow->m_oExtent.width;
    oViewport.height = -(float)pWindow->m_oExtent.height;
    oViewport.minDepth = 0.0f;
    oViewport.maxDepth = 1.0f;

    vkCmdSetViewport(pWindow->m_pCmdBuffers[uFrameIdx], 0, 1, &oViewport);

    VkRect2D oScissor{};
    oScissor.offset = { 0, 0 };
    oScissor.extent = pWindow->m_oExtent;

    vkCmdSetScissor(pWindow->m_pCmdBuffers[uFrameIdx], 0, 1, &oScissor);

    vkCmdBindDescriptorSets(pWindow->m_pCmdBuffers[uFrameIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hPipelineLayout, static_cast<uint32_t>(ResourceFrequency::MATERIAL), 1, &_pAPIRenderState->m_pDescSets[uFrameIdx], 0, NULL);

    s_oGlobalData.m_pUsingRenderState = _pAPIRenderState;
  }

  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    APIWindow* pWindow = _pAPIRenderState->m_pOwnerWindow;

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, uNumImages, pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    s_oVkDescriptorSetPool.ReturnElements(_pAPIRenderState->m_pDescSets);    

    vkDestroyDescriptorSetLayout(pWindow->m_hDevice, _pAPIRenderState->m_hDescSetLayout, NULL);    

    vkDestroyPipelineLayout(pWindow->m_hDevice, _pAPIRenderState->m_hPipelineLayout, NULL);

    vkDestroyPipeline(pWindow->m_hDevice, _pAPIRenderState->m_hGraphicsPipeline, NULL);

    s_oRenderStatePool.ReturnElement(_pAPIRenderState);
  }    

  uint32_t GetConstantBufferCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage)
  {
    uint32_t uCBufferCount = 0u;

    uint32_t uDescSetCount = 0u;
    SpvReflectDescriptorSet* aDescSets[4];
    
    GetDescSetReflection(_pAPIRenderState, _eStage, aDescSets, uDescSetCount);

    for (uint32_t i = 0u; i < uDescSetCount; i++)
    {
      SpvReflectDescriptorSet* pDescSet = aDescSets[i];

      if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
      {
        for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
        {
          SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];

          if (pDesc->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
          {
            uCBufferCount++;
          }
        }
      }
    }

    return uCBufferCount;
  }

  uint32_t GetTextureCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage)
  {
    return 0u;
  }

  std::string GetConstantBufferName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
  {
    uint32_t uDescSetCount = 0u;
    SpvReflectDescriptorSet* aDescSets[4];

    GetDescSetReflection(_pAPIRenderState, _eStage, aDescSets, uDescSetCount);

    uint32_t uCurrIdx = 0u;

    for (uint32_t i = 0u; i < uDescSetCount; i++)
    {
      SpvReflectDescriptorSet* pDescSet = aDescSets[i];

      if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
      {
        for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
        {
          SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];

          if (pDesc->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
          {
            if (uCurrIdx == _uIdx)
            {
              return pDesc->block.type_description->type_name;
            }

            uCurrIdx++;
          }
        }
      }
    }

    THROW_GENERIC_EXCEPTION("[API] Constant buffer not found by index");

    return "";
  }

  const char* GetTextureName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
  {
    return nullptr;
  }

  uint32_t GetConstantBufferMemberCount(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx)
  {
    uint32_t uDescSetCount = 0u;
    SpvReflectDescriptorSet* aDescSets[4];

    GetDescSetReflection(_pAPIRenderState, _eStage, aDescSets, uDescSetCount);

    uint32_t uCurrIdx = 0u;

    for (uint32_t i = 0u; i < uDescSetCount; i++)
    {
      SpvReflectDescriptorSet* pDescSet = aDescSets[i];

      if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
      {
        for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
        {
          SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];

          if (pDesc->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
          {
            if (uCurrIdx == _uIdx)
            {
              return pDesc->block.member_count;
            }

            uCurrIdx++;
          }
        }
      }
    }

    THROW_GENERIC_EXCEPTION("[API] Constant buffer not found by index");

    return 0u;
  }

  std::string GetConstantBufferMemberName(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {
    uint32_t uDescSetCount = 0u;
    SpvReflectDescriptorSet* aDescSets[4];

    GetDescSetReflection(_pAPIRenderState, _eStage, aDescSets, uDescSetCount);

    uint32_t uCurrIdx = 0u;

    for (uint32_t i = 0u; i < uDescSetCount; i++)
    {
      SpvReflectDescriptorSet* pDescSet = aDescSets[i];

      if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
      {
        for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
        {
          SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];

          if (pDesc->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
          {
            if (uCurrIdx == _uIdx)
            {
              if(pDesc->block.member_count > _uMemberIdx)
              {
                return pDesc->block.members[_uMemberIdx].name;
              }
              break;
            }

            uCurrIdx++;
          }
        }
      }
    }

    THROW_GENERIC_EXCEPTION("[API] Constant buffer member not found by index");

    return "";
  }

  size_t GetConstantBufferMemberSize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {

    uint32_t uDescSetCount = 0u;
    SpvReflectDescriptorSet* aDescSets[4];

    GetDescSetReflection(_pAPIRenderState, _eStage, aDescSets, uDescSetCount);

    uint32_t uCurrIdx = 0u;

    for (uint32_t i = 0u; i < uDescSetCount; i++)
    {
      SpvReflectDescriptorSet* pDescSet = aDescSets[i];

      if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
      {
        for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
        {
          SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];

          if (pDesc->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
          {
            if (uCurrIdx == _uIdx)
            {
              if (pDesc->block.member_count > _uMemberIdx)
              {
                return pDesc->block.members[_uMemberIdx].padded_size;
              }
              break;
            }

            uCurrIdx++;
          }
        }
      }
    }

    THROW_GENERIC_EXCEPTION("[API] Constant buffer member not found by index");

    return 0u;
  }

  ConstantBufferType GetConstantBufferMemberType(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
  {

    uint32_t uDescSetCount = 0u;
    SpvReflectDescriptorSet* aDescSets[4];

    GetDescSetReflection(_pAPIRenderState, _eStage, aDescSets, uDescSetCount);

    uint32_t uCurrIdx = 0u;

    for (uint32_t i = 0u; i < uDescSetCount; i++)
    {
      SpvReflectDescriptorSet* pDescSet = aDescSets[i];

      if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
      {
        for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
        {
          SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];

          if (pDesc->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
          {
            if (uCurrIdx == _uIdx)
            {
              if (pDesc->block.member_count > _uMemberIdx)
              {                
                SpvReflectTypeFlags uTypeFlags = pDesc->block.members[_uMemberIdx].type_description->type_flags;
                if ((uTypeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT) != 0u)
                {
                  switch (pDesc->block.members[_uMemberIdx].type_description->traits.numeric.vector.component_count)
                  {
                  case 0u:
                    return ConstantBufferType::SCALAR;
                    break;
                  case 2u:
                    return ConstantBufferType::VEC2;
                    break;
                  case 3u:
                    return ConstantBufferType::VEC3;
                    break;
                  case 4u:
                    return ConstantBufferType::VEC4;
                    break;
                  default:
                    break;
                  }
                }
              }
              break;
            }

            uCurrIdx++;
          }
        }
      }
    }

    THROW_GENERIC_EXCEPTION("[API] Constant buffer member type invalid ");


    return ConstantBufferType::NONE;
  }

  APIRenderSubState* CreateAPIRenderSubState(ResourceFrequency _eFrequency)
  {    
    APIRenderSubState* pSubState = s_oRenderSubStatePool.PullElement();    

    pSubState->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

    switch (_eFrequency)
    {
    case ResourceFrequency::MATERIAL_INSTANCE:
      pSubState->m_hDecSetLayout = pSubState->m_pOwnerWindow->m_hMatInstanceDescSetLayout;
      break;
    case ResourceFrequency::GLOBAL:
      pSubState->m_hDecSetLayout = pSubState->m_pOwnerWindow->m_hGlobalDescSetLayout;
      break;
    case ResourceFrequency::RENDER_STEP:
      pSubState->m_hDecSetLayout = pSubState->m_pOwnerWindow->m_hStepDescSetLayout;
      break;
    default:
      THROW_GENERIC_EXCEPTION("[API] [VK] Tried to create an APIRenderSubState with invalid ResourceFrequency")
      break;
    }

    uint32_t uNumImages = s_oGlobalData.m_pUsingWindow->m_uSwapchainImageCount;

    std::vector<VkDescriptorSetLayout> lstLayouts;
    for (int i = 0; i < uNumImages; i++)
    {
      lstLayouts.push_back(pSubState->m_hDecSetLayout);
    }

    VkDescriptorSetAllocateInfo oDescSetAllocInfo{};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = s_oGlobalData.m_pUsingWindow->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = lstLayouts.data();

    pSubState->m_pDescSets = s_oVkDescriptorSetPool.PullElements(uNumImages);

    VK_CHECK(vkAllocateDescriptorSets(s_oGlobalData.m_pUsingWindow->m_hDevice, &oDescSetAllocInfo, pSubState->m_pDescSets))

    return pSubState;
  }

  void BeginSubStateSetup(APIRenderSubState* _pSubState)
  {
    /*if (s_oGlobalData.m_pUsingSubState != nullptr)
    {
      THROW_GENERIC_EXCEPTION("[API] EndSubStateSetup was not called before a new BeginSubStateSetup call")
    }*/

    s_oGlobalData.m_pUsingSubState = _pSubState;
    s_oGlobalData.m_pUsingWindow = _pSubState->m_pOwnerWindow;    
  }

  void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo)
  {    

    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;    

    uint32_t uBinding = FindBinding(pWindow, _oBindInfo, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    if (s_oGlobalData.m_bRenderBegan)
    {
      VkDescriptorBufferInfo oBufferInfo{};
      oBufferInfo.buffer = _pCBuffer->m_pUniformBuffers[pWindow->m_uCurrFrameIdx];
      oBufferInfo.offset = 0;
      oBufferInfo.range = _uSize;

      s_oGlobalData.m_oDescSetUpdater.AddBufferInfo(std::move(oBufferInfo), uBinding, pWindow->m_uCurrFrameIdx, _oBindInfo.m_eStage);
    }
    else
    {
      uint32_t uNumImages = pWindow->m_uSwapchainImageCount;
      for (int i = 0; i < uNumImages; i++)
      {
        VkDescriptorBufferInfo oBufferInfo{};
        oBufferInfo.buffer = _pCBuffer->m_pUniformBuffers[i];
        oBufferInfo.offset = 0;
        oBufferInfo.range = _uSize;

        s_oGlobalData.m_oDescSetUpdater.AddBufferInfo(std::move(oBufferInfo), uBinding, i, _oBindInfo.m_eStage);
      }
    }
  }

  void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
  {    

    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

    uint32_t uNumImages = s_oGlobalData.m_bRenderBegan ? 1u : pWindow->m_uSwapchainImageCount;

    uint32_t uBinding = FindBinding(pWindow, _oBindInfo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    if (s_oGlobalData.m_bRenderBegan)
    {
      VkDescriptorImageInfo oImageInfo{};
      oImageInfo.sampler = _pTexture->m_hSampler;
      oImageInfo.imageView = _pTexture->m_hImageView;
      oImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      s_oGlobalData.m_oDescSetUpdater.AddImageInfo(std::move(oImageInfo), uBinding, pWindow->m_uCurrFrameIdx, _oBindInfo.m_eStage);
    }
    else
    {
      for (int i = 0; i < uNumImages; i++)
      {
        VkDescriptorImageInfo oImageInfo{};
        oImageInfo.sampler = _pTexture->m_hSampler;
        oImageInfo.imageView = _pTexture->m_hImageView;
        oImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        s_oGlobalData.m_oDescSetUpdater.AddImageInfo(std::move(oImageInfo), uBinding, i, _oBindInfo.m_eStage);
      }
    }
  }

  void EndSubStateSetup(ResourceFrequency _eFrequency)
  { 

    APIRenderSubState* pSubState = s_oGlobalData.m_pUsingSubState;    

    APIWindow* pWindow = pSubState->m_pOwnerWindow;

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    DescriptorSetLayoutBuilder* pLayoutBuilder = nullptr;

    switch (_eFrequency)
    {
    case ResourceFrequency::MATERIAL_INSTANCE:
      pLayoutBuilder = &pWindow->m_oMatInstanceLayoutBuilder;
      break;
    case ResourceFrequency::GLOBAL:
      pLayoutBuilder = &pWindow->m_oGlobalLayoutBuilder;
      break;
    case ResourceFrequency::RENDER_STEP:
      pLayoutBuilder = &pWindow->m_oStepLayoutBuilder;
      break;
    }        

    s_oGlobalData.m_oDescSetUpdater.Update(pWindow->m_hDevice, pSubState->m_pDescSets, uNumImages, pLayoutBuilder);

    s_oGlobalData.m_oDescSetUpdater.Clear();

    s_oGlobalData.m_pUsingSubState = nullptr;    
  }

  void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency)
  {
    APIWindow* pWindow = _pAPIRenderSubState->m_pOwnerWindow;    
    APIRenderState* pRenderState = s_oGlobalData.m_pUsingRenderState;
    const uint32_t uFrameIdx = pWindow->m_uCurrFrameIdx;
    vkCmdBindDescriptorSets(pWindow->m_pCmdBuffers[uFrameIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, pRenderState->m_hPipelineLayout, static_cast<uint32_t>(_eFrequency), 1, &_pAPIRenderSubState->m_pDescSets[uFrameIdx], 0, NULL);

    s_oGlobalData.m_pUsingSubState = _pAPIRenderSubState;
  }

  void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    APIWindow* pWindow = _pAPIRenderSubState->m_pOwnerWindow;

    const uint32_t uFrameIdx = pWindow->m_uCurrFrameIdx;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, 1, &pWindow->m_pInFlightFences[uFrameIdx], VK_TRUE, UINT64_MAX))

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;
    vkFreeDescriptorSets(pWindow->m_hDevice, pWindow->m_hDescPool, uNumImages, _pAPIRenderSubState->m_pDescSets);

    s_oVkDescriptorSetPool.ReturnElements(_pAPIRenderSubState->m_pDescSets);

    s_oRenderSubStatePool.ReturnElement(_pAPIRenderSubState);
  }

  // Drawing

  void WaitForNextImage(APIWindow* _pWindow)
  {
    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, _pWindow->m_uSwapchainImageCount, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))
  }

  int BeginDraw(APIWindow* _pWindow)
  {

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, 1, &_pWindow->m_pInFlightFences[uFrameIdx], VK_TRUE, UINT64_MAX))

    VkResult hResult = VK_SUCCESS; 
    
    if (!_pWindow->m_bResized)
    {
      hResult = vkAcquireNextImageKHR(_pWindow->m_hDevice, _pWindow->m_hSwapchain, UINT64_MAX, _pWindow->m_pImageAvailableSemaphores[uFrameIdx], VK_NULL_HANDLE, &_pWindow->m_uCurrSwapchainImageIdx);
    }

    if (hResult == VK_ERROR_OUT_OF_DATE_KHR || hResult == VK_SUBOPTIMAL_KHR || _pWindow->m_bResized)
    {
      _pWindow->m_bResized = false;     
      ImGui::EndFrame();
      RecreateSwapchain(_pWindow);      
      ImGui_ImplVulkan_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      SetupImGui(_pWindow);
      return 1;
    }
    else if (hResult != VK_SUCCESS)
    {
      VK_CHECK(hResult)
    }

    VK_CHECK(vkResetFences(_pWindow->m_hDevice, 1, &_pWindow->m_pInFlightFences[uFrameIdx]))

    VK_CHECK(vkResetCommandBuffer(_pWindow->m_pCmdBuffers[uFrameIdx], 0))

    VkCommandBufferBeginInfo oCommandBufferBeginInfo {};
    oCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    oCommandBufferBeginInfo.flags = 0u;
    oCommandBufferBeginInfo.pInheritanceInfo = NULL;

    VK_CHECK(vkBeginCommandBuffer(_pWindow->m_pCmdBuffers[uFrameIdx], &oCommandBufferBeginInfo))   

    s_oGlobalData.m_pUsingWindow = _pWindow;

    s_oGlobalData.m_bRenderBegan = true;

    return 0;
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount, const void* _pConstantData, uint32_t _uConstantSize)
  {

    APIWindow* pWindow = _pMesh->m_pOwnerWindow;

    VkCommandBuffer& hCmdBuffer = pWindow->m_pCmdBuffers[pWindow->m_uCurrFrameIdx];

    APIRenderState* pRenderState = s_oGlobalData.m_pUsingRenderState;

    VkBuffer vertexBuffers[] = { _pMesh->m_hVertexBuffer };
    VkDeviceSize offsets[] = { 0u };
    vkCmdBindVertexBuffers(hCmdBuffer, 0u, 1u, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(hCmdBuffer, _pMesh->m_hIndexBuffer, 0u, VK_INDEX_TYPE_UINT16);

    vkCmdPushConstants(hCmdBuffer, pRenderState->m_hPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, _uConstantSize, _pConstantData);

    vkCmdDrawIndexed(hCmdBuffer, _uIndexCount, 1u, 0u, 0u, 0u);

  }

  void EndDraw(APIWindow* _pWindow)
  {    

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    //vkCmdEndRenderPass(_pWindow->m_pCmdBuffers[uFrameIdx]);

    VK_CHECK(vkEndCommandBuffer(_pWindow->m_pCmdBuffers[uFrameIdx]))

    VkSemaphore aWaitSemaphores[] = { _pWindow->m_pImageAvailableSemaphores[uFrameIdx] };
    VkPipelineStageFlags aWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore aSignalSemaphores[] = { _pWindow->m_pRenderFinishedSemaphores[uFrameIdx] };

    VkSubmitInfo oSubmitInfo{};
    oSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    oSubmitInfo.waitSemaphoreCount = 1;
    oSubmitInfo.pWaitSemaphores = aWaitSemaphores;
    oSubmitInfo.pWaitDstStageMask = aWaitStages;
    oSubmitInfo.commandBufferCount = 1u;
    oSubmitInfo.pCommandBuffers = &_pWindow->m_pCmdBuffers[uFrameIdx];
    oSubmitInfo.signalSemaphoreCount = 1u;
    oSubmitInfo.pSignalSemaphores = aSignalSemaphores;

    VK_CHECK(vkQueueSubmit(_pWindow->m_hRenderQueue, 1, &oSubmitInfo, _pWindow->m_pInFlightFences[uFrameIdx]))

    VkSwapchainKHR swapChains[] = { _pWindow->m_hSwapchain };

    VkPresentInfoKHR oPresentInfo{};
    oPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    oPresentInfo.waitSemaphoreCount = 1;
    oPresentInfo.pWaitSemaphores = aSignalSemaphores;
    oPresentInfo.swapchainCount = 1;
    oPresentInfo.pSwapchains = swapChains;
    oPresentInfo.pImageIndices = &_pWindow->m_uCurrSwapchainImageIdx;
    oPresentInfo.pResults = NULL;

    VK_CHECK(vkQueuePresentKHR(_pWindow->m_hPresentQueue, &oPresentInfo))

    //_pWindow->m_uCurrSwapchainImageIdx = (_pWindow->m_uCurrSwapchainImageIdx + 1u) % _pWindow->m_uSwapchainImageCount;

    s_oGlobalData.m_pUsingSubState = nullptr;

    _pWindow->m_uCurrFrameIdx = (_pWindow->m_uCurrFrameIdx + 1u) % _pWindow->m_uSwapchainImageCount;
  }

  // Misc

  void ImGuiNewFrame()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

}
}

#endif