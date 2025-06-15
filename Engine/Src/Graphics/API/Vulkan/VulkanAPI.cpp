
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
#include "File/InFile.h"

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
    s_oVkImageViewPool.Initialize();

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

  uint32_t GetMaxMsaaSamples()
  {
    return static_cast<uint32_t>(s_oGlobalData.m_eMaxMsaaSampleCount);
  }

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow, uint32_t _uMsaaSamples)
  {

    bool bMsaa = _uMsaaSamples > 1u;

    APIWindow* pWindow = new APIWindow();

    pWindow->m_pGlfwWindow = _pGlfwWindow;

    pWindow->m_eMsaaSamples = static_cast<VkSampleCountFlagBits>(_uMsaaSamples);

    if (_uMsaaSamples > static_cast<uint32_t>(s_oGlobalData.m_eMaxMsaaSampleCount))
    {
      pWindow->m_eMsaaSamples = s_oGlobalData.m_eMaxMsaaSampleCount;
    }

    CreateLogicalDevice(pWindow);
    RetrieveQueues(pWindow);
    CreateSurface(pWindow, _pGlfwWindow);
    CreateCommandBuffers(pWindow);
    CreateSwapchain(pWindow);
    if (bMsaa)
    {
      CreateColorBuffer(pWindow);
    }
    CreateDepthBuffer(pWindow);
    CreateSyncObjects(pWindow);
    CreateDescriptorPool(pWindow);
    CreateGlobalDescriptorLayout(pWindow);

    CreateRenderPass(pWindow, 1u, pWindow->m_eSwapchainFormat, true, VK_FORMAT_D32_SFLOAT, pWindow->m_eMsaaSamples, pWindow->m_hRenderPass, false);

    pWindow->m_pFramebuffers = new VkFramebuffer[pWindow->m_uSwapchainImageCount];

    for (int i = 0; i < pWindow->m_uSwapchainImageCount; i++)
    {

      uint32_t uAttachmentCount = 0u;

      VkImageView aImageViews[3];
      aImageViews[uAttachmentCount++] = bMsaa ? pWindow->m_hColorImageView : pWindow->m_pSwapChainImageViews[i];
      if (bMsaa)
      {
        aImageViews[uAttachmentCount++] = pWindow->m_pSwapChainImageViews[i];
      }
      aImageViews[uAttachmentCount++] = pWindow->m_hDepthImageView;
      

      VkFramebufferCreateInfo oFramebufferInfo = {};
      oFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      oFramebufferInfo.attachmentCount = uAttachmentCount;
      oFramebufferInfo.pAttachments = &aImageViews[0];
      oFramebufferInfo.renderPass = pWindow->m_hRenderPass;
      oFramebufferInfo.width = pWindow->m_oExtent.width;
      oFramebufferInfo.height = pWindow->m_oExtent.height;
      oFramebufferInfo.layers = 1;

      VK_CHECK(vkCreateFramebuffer(pWindow->m_hDevice, &oFramebufferInfo, NULL, &pWindow->m_pFramebuffers[i]))
    }    

    pWindow->m_uCurrFrameIdx = 0u;

    // Create dummy texture
    SamplerConfig oDummySamplerConfig = {};
    uint8_t aDummyBuffer[4] = { 255u, 20u, 0u, 255u};    
    void* pDummyBuffer = &(aDummyBuffer[0]);
    pWindow->m_pDummyTexture = CreateAPITexture(pWindow, &pDummyBuffer, 1u, 1u, ImageFormat::R8G8B8A8, 1u, 1u, TextureUsage::SHADER_RESOURCE, oDummySamplerConfig, false);

    // ImGui stuff

    SetupImGui(pWindow);

    return pWindow;

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

  uint32_t GetWindowMSAASamples(APIWindow* _pWindow)
  {
    return _pWindow->m_eMsaaSamples;
  }

  void ClearDefaultRenderTarget(APIWindow* _pWindow)
  {
    uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    VkCommandBuffer hCmdBuffer = _pWindow->m_pCmdBuffers[uFrameIdx];

    // Depth buffer

    TransitionImageLayout(hCmdBuffer
      , _pWindow->m_hDepthImage
      , VK_FORMAT_D32_SFLOAT
      , 0u, 1u
      , 0u, 1u
      , VK_IMAGE_ASPECT_DEPTH_BIT
      , VK_IMAGE_LAYOUT_UNDEFINED
      , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageSubresourceRange oRange = {};
    oRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    oRange.baseMipLevel = 0;
    oRange.levelCount = VK_REMAINING_MIP_LEVELS;
    oRange.baseArrayLayer = 0;
    oRange.layerCount = 1;
    
    VkClearDepthStencilValue oDepthClearColor = { 1.f, 0.f };
    vkCmdClearDepthStencilImage(hCmdBuffer, _pWindow->m_hDepthImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oDepthClearColor, 1u, &oRange);

    TransitionImageLayout(hCmdBuffer
      , _pWindow->m_hDepthImage
      , VK_FORMAT_D32_SFLOAT
      , 0u, 1u
      , 0u, 1u
      , VK_IMAGE_ASPECT_DEPTH_BIT
      , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
      , VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // Color buffer

    if (_pWindow->m_hColorImage != VK_NULL_HANDLE)
    {
      TransitionImageLayout(hCmdBuffer
        , _pWindow->m_hColorImage
        , _pWindow->m_eSwapchainFormat
        , 0u, 1u
        , 0u, 1u
        , VK_IMAGE_ASPECT_COLOR_BIT
        , VK_IMAGE_LAYOUT_UNDEFINED
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      oRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      oRange.baseMipLevel = 0;
      oRange.levelCount = VK_REMAINING_MIP_LEVELS;
      oRange.baseArrayLayer = 0;
      oRange.layerCount = 1;

      VkClearColorValue oClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
      vkCmdClearColorImage(hCmdBuffer, _pWindow->m_hColorImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oClearColor, 1u, &oRange);

      TransitionImageLayout(hCmdBuffer
        , _pWindow->m_hColorImage
        , _pWindow->m_eSwapchainFormat
        , 0u, 1u
        , 0u, 1u
        , VK_IMAGE_ASPECT_COLOR_BIT
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        , VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
    else
    {
      TransitionImageLayout(hCmdBuffer
        , _pWindow->m_pSwapchainImages[uFrameIdx]
        , _pWindow->m_eSwapchainFormat
        , 0u, 1u
        , 0u, 1u
        , VK_IMAGE_ASPECT_COLOR_BIT
        , VK_IMAGE_LAYOUT_UNDEFINED
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
       
      oRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      oRange.baseMipLevel = 0;
      oRange.levelCount = VK_REMAINING_MIP_LEVELS;
      oRange.baseArrayLayer = 0;
      oRange.layerCount = 1;

      VkClearColorValue oClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
      vkCmdClearColorImage(hCmdBuffer, _pWindow->m_pSwapchainImages[uFrameIdx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &oClearColor, 1u, &oRange);

      TransitionImageLayout(hCmdBuffer
        , _pWindow->m_pSwapchainImages[uFrameIdx]
        , _pWindow->m_eSwapchainFormat
        , 0u, 1u
        , 0u, 1u
        , VK_IMAGE_ASPECT_COLOR_BIT
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        , VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
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

    VkViewport oViewport{};
    oViewport.x = 0.0f;
    oViewport.y = (float)_pWindow->m_oExtent.height;
    oViewport.width = (float)_pWindow->m_oExtent.width;
    oViewport.height = -(float)_pWindow->m_oExtent.height;
    oViewport.minDepth = 0.0f;
    oViewport.maxDepth = 1.0f;

    vkCmdSetViewport(_pWindow->m_pCmdBuffers[uFrameIdx], 0, 1, &oViewport);

    VkRect2D oScissor{};
    oScissor.offset = { 0, 0 };
    oScissor.extent = _pWindow->m_oExtent;

    vkCmdSetScissor(_pWindow->m_pCmdBuffers[uFrameIdx], 0, 1, &oScissor);
  }

  void UnbindDefaultRenderTarget(APIWindow* _pWindow)
  {
    
    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    // Record dear imgui primitives into command buffer
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _pWindow->m_pCmdBuffers[uFrameIdx]);

    vkCmdEndRenderPass(_pWindow->m_pCmdBuffers[uFrameIdx]);    
  }

  void DestroyAPIWindow(APIWindow* _pWindow)
  {

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    DestroyAPITexture(_pWindow, _pWindow->m_pDummyTexture);

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

  // Mesh

  APIMesh* CreateAPIMesh(const APIWindow* _pWindow,  const void* _pVertexData, size_t _uVertexDataSize, const void* _pIndexData, size_t _uIndexDataSize)
  {
    APIMesh* pMesh = s_oMeshPool.PullElement();

    VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);

    VkBuffer hVertexStagingBuffer;
    VkDeviceMemory hVertexStagingBufferMemory;
    VkBuffer hIndexStagingBuffer;
    VkDeviceMemory hIndexStagingBufferMemory;

    // Create vertex buffer
    {      
      VkMemoryPropertyFlags uProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      VkBufferUsageFlags uUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

      CreateBuffer(_pWindow, _uVertexDataSize, uUsage, uProperties, hVertexStagingBuffer, hVertexStagingBufferMemory);
      SetBufferData(_pWindow, _pVertexData, _uVertexDataSize, hVertexStagingBufferMemory);

      uProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      uUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

      CreateBuffer(_pWindow, _uVertexDataSize, uUsage, uProperties, pMesh->m_hVertexBuffer, pMesh->m_hVertexBufferMemory);

      CopyBuffer(hCmdBuffer, hVertexStagingBuffer, pMesh->m_hVertexBuffer, _uVertexDataSize);      
    }

    // Create index buffer
    {
      VkMemoryPropertyFlags uProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      VkBufferUsageFlags uUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

      CreateBuffer(_pWindow, _uIndexDataSize, uUsage, uProperties, hIndexStagingBuffer, hIndexStagingBufferMemory);
      SetBufferData(_pWindow, _pIndexData, _uIndexDataSize, hIndexStagingBufferMemory);

      uProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      uUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

      CreateBuffer(_pWindow, _uIndexDataSize, uUsage, uProperties, pMesh->m_hIndexBuffer, pMesh->m_hIndexBufferMemory);

      CopyBuffer(hCmdBuffer, hIndexStagingBuffer, pMesh->m_hIndexBuffer, _uIndexDataSize);      
    }

    EndTempCmdBuffer(_pWindow, hCmdBuffer);

    vkDestroyBuffer(_pWindow->m_hDevice, hVertexStagingBuffer, NULL);
    vkFreeMemory(_pWindow->m_hDevice, hVertexStagingBufferMemory, NULL);
    vkDestroyBuffer(_pWindow->m_hDevice, hIndexStagingBuffer, NULL);
    vkFreeMemory(_pWindow->m_hDevice, hIndexStagingBufferMemory, NULL);

    return pMesh;
  }

  void DestroyAPIMesh(const APIWindow* _pWindow, APIMesh* _pMesh)
  {

    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    DestroyBuffer(_pWindow, _pMesh->m_hVertexBuffer, _pMesh->m_hVertexBufferMemory);
    DestroyBuffer(_pWindow, _pMesh->m_hIndexBuffer, _pMesh->m_hIndexBufferMemory);

    s_oMeshPool.ReturnElement(_pMesh);
  }

  // Constant buffer

  APIConstantBuffer* CreateAPIConstantBuffer(const APIWindow* _pWindow, size_t _uSize)
  {
    APIConstantBuffer* pCBuffer = s_oConstantBufferPool.PullElement();        
    
    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    // Create buffers

    pCBuffer->m_pUniformBuffers = s_oVkBufferPool.PullElements(uNumImages);
    pCBuffer->m_pUniformBuffersMemory = s_oVkDeviceMemoryPool.PullElements(uNumImages);
    pCBuffer->m_pUniformBuffersMapped = s_oVoidPtrPool.PullElements(uNumImages);

    for (uint32_t i = 0; i < uNumImages; i++)
    {
      CreateBuffer(_pWindow, _uSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        pCBuffer->m_pUniformBuffers[i],
        pCBuffer->m_pUniformBuffersMemory[i]);

      VK_CHECK(vkMapMemory(_pWindow->m_hDevice, pCBuffer->m_pUniformBuffersMemory[i], 0u, _uSize, 0u, &pCBuffer->m_pUniformBuffersMapped[i]))
    }

    return pCBuffer;
  }

  void UpdateAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize)
  {    
    memcpy(_pCBuffer->m_pUniformBuffersMapped[_pWindow->m_uCurrFrameIdx], _pData, _uSize);
  }

  void BindAPIConstantBuffer(const APIWindow* /*_pWindow*/, APIConstantBuffer* /*_pCbuffer*/)
  {
    
  }

  void DestroyAPIConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer)
  {       
    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))    

    for (uint32_t i = 0; i < uNumImages; i++)
    {
      vkUnmapMemory(_pWindow->m_hDevice, _pCBuffer->m_pUniformBuffersMemory[i]);
      DestroyBuffer(_pWindow, _pCBuffer->m_pUniformBuffers[i], _pCBuffer->m_pUniformBuffersMemory[i]);
    }

    s_oVkBufferPool.ReturnElements(_pCBuffer->m_pUniformBuffers);
    s_oVkDeviceMemoryPool.ReturnElements(_pCBuffer->m_pUniformBuffersMemory);
    s_oVoidPtrPool.ReturnElements(_pCBuffer->m_pUniformBuffersMapped);

    s_oConstantBufferPool.ReturnElement(_pCBuffer);
  }

  // Texture

  APITexture* CreateAPITexture(const APIWindow* _pWindow, const void* const* _ppData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels, uint32_t _uMsaaSamples, uint32_t _uUsage, const SamplerConfig& _rSamplerConfig, bool _bIsCubemap)
  {

    APITexture* pTexture = s_oTexturePool.PullElement();

    VkBuffer hStagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory hStagingBufferMemory;       

    size_t uSize = _uWidth * _uHeight * GetImageFormatSize(_eFormat);

    if (_bIsCubemap)
    {
      uSize *= 6u;
    }

    pTexture->m_eFormat = GetVKFormat(_eFormat);

    uint32_t uMaxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_uWidth, _uWidth)))) + 1;
    _uMipLevels = _uMipLevels == 0u? uMaxMipLevels : std::min(_uMipLevels, uMaxMipLevels);

    uint32_t uLayers = _bIsCubemap ? 6u : 1u;

    pTexture->m_uMipLevels = _uMipLevels;
    pTexture->m_uLayers = uLayers;
    pTexture->m_uWidth = _uWidth;
    pTexture->m_uHeight = _uHeight;

    if (_ppData != nullptr)
    {
      CreateBuffer(_pWindow
        , uSize
        , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        , hStagingBuffer
        , hStagingBufferMemory);

      if (_bIsCubemap)
      {
        size_t uSizePerLayer = uSize / 6u;
        void* pDeviceData;
        VK_CHECK(vkMapMemory(_pWindow->m_hDevice, hStagingBufferMemory, 0, uSize, 0, &pDeviceData))
        for (size_t i = 0u; i < 6u; i++)
        {          
          memcpy(static_cast<char*>(pDeviceData) + uSizePerLayer * i, *(_ppData + i), uSizePerLayer);
        }
        vkUnmapMemory(_pWindow->m_hDevice, hStagingBufferMemory);
      }
      else
      {
        SetBufferData(_pWindow, *_ppData, uSize, hStagingBufferMemory);
      }
    }

    VkImageUsageFlags uUsageFlags = GetVkTextureUsage(_uUsage);

    if (_ppData != nullptr)
    {
      uUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if (_uMipLevels > 1u)
    {
      uUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    CreateImage(_pWindow
      , _uWidth
      , _uHeight
      , pTexture->m_eFormat
      , _uMipLevels
      , uLayers
      , static_cast<VkSampleCountFlagBits>(_uMsaaSamples)
      , VK_IMAGE_TILING_OPTIMAL
      , uUsageFlags
      , _bIsCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u      
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

    VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);

    if (_ppData != nullptr)
    {
      TransitionImageLayout(hCmdBuffer
        , pTexture->m_hImage
        , pTexture->m_eFormat
        , 0u, _uMipLevels
        , 0u, uLayers
        , uAspectFlags
        , VK_IMAGE_LAYOUT_UNDEFINED
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      CopyBufferToImage(hCmdBuffer, hStagingBuffer, pTexture->m_hImage, _uWidth, _uHeight, uLayers);


      // When we generate mipmaps, we also do the transition layout
      if (_uMipLevels > 1u)
      {
        GenerateMipmaps(hCmdBuffer, pTexture->m_hImage, _uWidth, _uHeight, _uMipLevels, uLayers);
      }
      // If no mipmaps, perform layout transition
      else
      {
        TransitionImageLayout(hCmdBuffer
          , pTexture->m_hImage
          , pTexture->m_eFormat
          , 0u, _uMipLevels
          , 0u, uLayers
          , uAspectFlags
          , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
          , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
      }

      SetTextureLayoutAllMipLevels(pTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    else
    {

      VkImageLayout eFinalLayout = ((_uUsage & TextureUsage::COLOR_TARGET) != 0)
        ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      TransitionImageLayout(hCmdBuffer
        , pTexture->m_hImage
        , pTexture->m_eFormat
        , 0u, _uMipLevels
        , 0u, uLayers
        , uAspectFlags
        , VK_IMAGE_LAYOUT_UNDEFINED
        , eFinalLayout);
      
      SetTextureLayoutAllMipLevels(pTexture, eFinalLayout);
    }

    EndTempCmdBuffer(_pWindow, hCmdBuffer);

    if (hStagingBuffer != VK_NULL_HANDLE)
    {
      DestroyBuffer(_pWindow, hStagingBuffer, hStagingBufferMemory);
    }

    CreateImageView(_pWindow
      , pTexture->m_hImage
      , pTexture->m_eFormat
      , _uMipLevels
      , uLayers
      , _bIsCubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D
      , uAspectFlags
      , pTexture->m_hImageView);

    if ((_uUsage & TextureUsage::SHADER_RESOURCE) != 0u)
    {
      CreateTextureSampler(_pWindow, pTexture, _uMipLevels, _rSamplerConfig);
    }      

    return pTexture;
  }

  void GenerateMipMaps(const APIWindow* _pWindow, APITexture* _pTexture)
  {
    if (_pTexture->m_uMipLevels > 1u)
    {            
      VkCommandBuffer hCmdBuffer = _pWindow->m_pCmdBuffers[_pWindow->m_uCurrFrameIdx];

      TransitionTextureImageLayout(hCmdBuffer
        , _pTexture
        , 0u, _pTexture->m_uMipLevels
        , 0u, _pTexture->m_uLayers
        , VK_IMAGE_ASPECT_COLOR_BIT
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      GenerateMipmaps(hCmdBuffer, _pTexture->m_hImage, _pTexture->m_uWidth, _pTexture->m_uHeight, _pTexture->m_uMipLevels, _pTexture->m_uLayers);
      
      SetTextureLayoutAllMipLevels(_pTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
  }

  void BindAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
  {    
    uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    TransitionTextureImageLayout(
      _pWindow->m_pCmdBuffers[uFrameIdx]
      , _pTexture
      , 0u, _pTexture->m_uMipLevels
      , 0u, _pTexture->m_uLayers
      , VK_IMAGE_ASPECT_COLOR_BIT
      , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }

  void UnbindAPITexture(const APIWindow* /*_pWindow*/, APITexture* /*_pTexture*/)
  {

  }

  void ClearAPITexture(const APIWindow* _pWindow, APITexture* _pTexture, TextureUsage _eUsage)
  {    

    uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

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

    VkCommandBuffer hCmdBuffer = _pWindow->m_pCmdBuffers[uFrameIdx];

    TransitionTextureImageLayout(hCmdBuffer,
      _pTexture,
      0u, _pTexture->m_uMipLevels, 
      0u, 1u, 
      uAspectFlags, 
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageSubresourceRange oRange = {};
    oRange.aspectMask = uAspectFlags;
    oRange.baseMipLevel = 0;
    oRange.levelCount = VK_REMAINING_MIP_LEVELS;
    oRange.baseArrayLayer = 0;
    oRange.layerCount = 1;

    if (_eUsage == TextureUsage::DEPTH_TARGET)
    {
      VkClearDepthStencilValue oClearColor = { 1.f, 0.f };
      vkCmdClearDepthStencilImage(_pWindow->m_pCmdBuffers[uFrameIdx], 
        _pTexture->m_hImage, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        &oClearColor, 
        1u, 
        &oRange);
    }
    else
    {
      VkClearColorValue oClearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; 
      vkCmdClearColorImage(hCmdBuffer, 
        _pTexture->m_hImage, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        &oClearColor, 
        1u, 
        &oRange);
    }

    VkImageLayout eFinalLayout = _eUsage == TextureUsage::COLOR_TARGET 
      ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 
      : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    TransitionTextureImageLayout(hCmdBuffer,
      _pTexture,
      0u, _pTexture->m_uMipLevels,
      0u, 1u,
      uAspectFlags,
      eFinalLayout);    
  }

  void DestroyAPITexture(const APIWindow* _pWindow, APITexture* _pTexture)
  {
    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    vkDestroySampler(_pWindow->m_hDevice, _pTexture->m_hSampler, NULL);
    vkDestroyImageView(_pWindow->m_hDevice, _pTexture->m_hImageView, NULL);
    vkDestroyImage(_pWindow->m_hDevice, _pTexture->m_hImage, NULL);
    vkFreeMemory(_pWindow->m_hDevice, _pTexture->m_hMemory, NULL);

    s_oTexturePool.ReturnElement(_pTexture);
  }

  // RenderTarget

  APIRenderTarget* CreateAPIRenderTarget()
  {
    APIRenderTarget* pRenderTarget = s_oRenderTargetPool.PullElement();    

    return pRenderTarget;
  }

  void BeginRenderTargetSetup(APIRenderTarget* _pRenderTarget, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, ImageFormat _eDepthStencilFormat, uint32_t _uMsaaSamples, bool _isCubemap)
  {
    s_oGlobalData.m_oRenderTargetBuilder.SetRenderTarget(_pRenderTarget);
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

  void EndRenderTargetSetup(const APIWindow* _pWindow)
  {
    s_oGlobalData.m_oRenderTargetBuilder.Build(_pWindow);
    s_oGlobalData.m_oRenderTargetBuilder.Clear();
  }

  void ClearAPIRenderTarget(const APIWindow* /*_pWindow*/, APIRenderTarget* /*_pRenderTarget*/)
  {  
  }

  void BindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {

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

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    vkCmdBeginRenderPass(_pWindow->m_pCmdBuffers[uFrameIdx], &oRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport oViewport{};
    oViewport.x = 0.0f;
    oViewport.y = (float)oExtent.height;
    oViewport.width = (float)oExtent.width;
    oViewport.height = -(float)oExtent.height;
    oViewport.minDepth = 0.0f;
    oViewport.maxDepth = 1.0f;

    vkCmdSetViewport(_pWindow->m_pCmdBuffers[uFrameIdx], 0, 1, &oViewport);

    VkRect2D oScissor{};
    oScissor.offset = { 0, 0 };
    oScissor.extent = oExtent;

    vkCmdSetScissor(_pWindow->m_pCmdBuffers[uFrameIdx], 0, 1, &oScissor);
  }

  void UnbindAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {    
    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    vkCmdEndRenderPass(_pWindow->m_pCmdBuffers[uFrameIdx]);    

    for (uint32_t i = 0u; i < 4u; i++)
    {
      if (_pRenderTarget->m_aColorTextures[i] != nullptr)
      {
        _pRenderTarget->m_aColorTextures[i]->m_aCurrLayouts[0] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      }

      if (_pRenderTarget->m_aResolveTextures[i] != nullptr)
      {
        _pRenderTarget->m_aResolveTextures[i]->m_aCurrLayouts[0] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      }
    }

    if (_pRenderTarget->m_pDepthTexture != nullptr)
    {      
      _pRenderTarget->m_pDepthTexture->m_aCurrLayouts[0] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
  }

  void DestroyAPIRenderTarget(const APIWindow* _pWindow, APIRenderTarget* _pRenderTarget)
  {
    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    vkDestroyFramebuffer(_pWindow->m_hDevice, _pRenderTarget->m_hFrameBuffer, nullptr);

    vkDestroyRenderPass(_pWindow->m_hDevice, _pRenderTarget->m_hRenderPass, nullptr);

    if (_pRenderTarget->m_pImageViews)
    {
      for (uint32_t i = 0; i < _pRenderTarget->m_uAttachmentCount; i++)
      {
        vkDestroyImageView(_pWindow->m_hDevice, _pRenderTarget->m_pImageViews[i], nullptr);
      }
      s_oVkImageViewPool.ReturnElements(_pRenderTarget->m_pImageViews);
    }

    s_oRenderTargetPool.ReturnElement(_pRenderTarget);
  }

  // Render state

  APIRenderState* CreateAPIRenderState(const APIWindow* _pWindow, const RenderStateInfo& _oInfo, APIRenderTarget* _pRenderTarget, uint32_t _uMsaaSamples)
  {

    VkRenderPass hRenderPass = VK_NULL_HANDLE;

    if (_pRenderTarget)
    {
      hRenderPass = _pRenderTarget->m_hRenderPass;
    }    
    else
    {
      hRenderPass = _pWindow->m_hRenderPass;
    }

    APIRenderState* pRenderState = s_oRenderStatePool.PullElement();

    uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;    

    size_t uSlashPosVS = _oInfo.m_sVSFilename.find_last_of('/');    
    size_t uSlashPosPS = _oInfo.m_sPSFilename.find_last_of('/');

    size_t uDotPosVS = _oInfo.m_sVSFilename.find_last_of('.');    
    size_t uDotPosPS = _oInfo.m_sPSFilename.find_last_of('.');

    std::string sVSPath = _oInfo.m_sVSFilename.substr(0, uSlashPosVS) + "/VK";    
    std::string sPSPath = _oInfo.m_sPSFilename.substr(0, uSlashPosPS) + "/VK";

    std::string sVSFilename = _oInfo.m_sVSFilename.substr(uSlashPosVS, uDotPosVS - uSlashPosVS) + ".spv";    
    std::string sPSFilename = _oInfo.m_sPSFilename.substr(uSlashPosPS, uDotPosPS - uSlashPosPS) + ".spv";    

    file::InFile oVSFile((sVSPath + sVSFilename).c_str());    
    file::InFile oPSFile((sPSPath + sPSFilename).c_str()); 

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

    owner_ptr<file::InFile> pGSFile = nullptr;

    if (!_oInfo.m_sGSFilename.empty())
    {
      size_t uSlashPosGS = _oInfo.m_sGSFilename.find_last_of('/');
      size_t uDotPosGS = _oInfo.m_sGSFilename.find_last_of('.');
      std::string sGSPath = _oInfo.m_sGSFilename.substr(0, uSlashPosGS) + "/VK";
      std::string sGSFilename = _oInfo.m_sGSFilename.substr(uSlashPosGS, uDotPosGS - uSlashPosGS) + ".spv";
      pGSFile = Factory::Create<file::InFile>((sGSPath + sGSFilename).c_str());

      eResult = spvReflectCreateShaderModule(pGSFile->GetSize(), static_cast<void*>(pGSFile->GetData()), &(pRenderState->m_oGeomReflection));
      if (eResult != SPV_REFLECT_RESULT_SUCCESS)
      {
        THROW_GENERIC_EXCEPTION("[API] Error: Unable to load geometry shader reflection")
      }

      ReflectSetLayouts(pRenderState->m_oGeomReflection, pRenderState->m_oMaterialLayoutBuilder);
    }    
    
    pRenderState->m_hDescSetLayout = pRenderState->m_oMaterialLayoutBuilder.Build(_pWindow->m_hDevice);

    std::vector<VkDescriptorSetLayout> lstLayouts;
    for (int i = 0; i < uNumImages; i++)
    {
      lstLayouts.push_back(pRenderState->m_hDescSetLayout);
    }

    // Allocate material substate
    VkDescriptorSetAllocateInfo oDescSetAllocInfo {};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = _pWindow->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = lstLayouts.data();

    pRenderState->m_pDescSets = s_oVkDescriptorSetPool.PullElements(uNumImages);

    VK_CHECK(vkAllocateDescriptorSets(_pWindow->m_hDevice, &oDescSetAllocInfo, pRenderState->m_pDescSets))
    
    CreatePipeline(_pWindow, oVSFile, oPSFile, pGSFile.get(), _oInfo, hRenderPass, static_cast<VkSampleCountFlagBits>(_uMsaaSamples), pRenderState);
    
    return pRenderState;
  }

  void BeginRenderStateSetup(APIRenderState* _pAPIRenderState)
  {

    /*if (s_oGlobalData.m_pUsingRenderState != nullptr)
    {
      THROW_GENERIC_EXCEPTION("[API] EndRenderStateSetup was not called before a new BeginRenderStateSetup call")
    }*/

    s_oGlobalData.m_oDescSetUpdater.SetDescriptorSets(_pAPIRenderState->m_pDescSets);
    s_oGlobalData.m_oDescSetUpdater.SetLayoutBuilder(&_pAPIRenderState->m_oMaterialLayoutBuilder);
  }

  void RenderStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState)
  {
    uint32_t uBinding = _pRenderState->m_oMaterialLayoutBuilder.FindBinding(_oBindInfo.m_sName, GetVkShaderFlags(_oBindInfo.m_uStageFlags), VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    AddImageToDescSetUpdater(_pWindow, _pTexture, uBinding, _oBindInfo.m_uStageFlags);
  }

  void RenderStateSetupConstantBuffer(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo, const APIRenderState* _pRenderState)
  {
    uint32_t uBinding = _pRenderState->m_oMaterialLayoutBuilder.FindBinding(_oBindInfo.m_sName, GetVkShaderFlags(_oBindInfo.m_uStageFlags), VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    AddBufferToDescSetUpdater(_pWindow, _pCBuffer, _uSize, uBinding, _oBindInfo.m_uStageFlags);
  }

  void EndRenderStateSetup(const APIWindow* _pWindow)
  {
    s_oGlobalData.m_oDescSetUpdater.Update(_pWindow);
    s_oGlobalData.m_oDescSetUpdater.Clear();
  }

  void BindAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState)
  {
    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    vkCmdBindPipeline(_pWindow->m_pCmdBuffers[uFrameIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hGraphicsPipeline);        

    vkCmdBindDescriptorSets(_pWindow->m_pCmdBuffers[uFrameIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hPipelineLayout, static_cast<uint32_t>(ResourceFrequency::MATERIAL), 1, &_pAPIRenderState->m_pDescSets[uFrameIdx], 0, NULL);
  }

  void DestroyAPIRenderState(const APIWindow* _pWindow, APIRenderState* _pAPIRenderState)
  {
    const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, uNumImages, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))

    s_oVkDescriptorSetPool.ReturnElements(_pAPIRenderState->m_pDescSets);    

    vkDestroyDescriptorSetLayout(_pWindow->m_hDevice, _pAPIRenderState->m_hDescSetLayout, NULL);    

    vkDestroyPipelineLayout(_pWindow->m_hDevice, _pAPIRenderState->m_hPipelineLayout, NULL);

    vkDestroyPipeline(_pWindow->m_hDevice, _pAPIRenderState->m_hGraphicsPipeline, NULL);

    s_oRenderStatePool.ReturnElement(_pAPIRenderState);
  }    

  // SHader reflection

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
                    if (pDesc->block.members[_uMemberIdx].type_description->traits.numeric.matrix.row_count == 3)
                    {
                      return ConstantBufferType::MAT3;
                    }
                    else
                    {
                      return ConstantBufferType::VEC3;
                    }
                    break;
                  case 4u:
                    if (pDesc->block.members[_uMemberIdx].type_description->traits.numeric.matrix.row_count == 4)
                    {
                      return ConstantBufferType::MAT4;
                    }
                    else
                    {
                      return ConstantBufferType::VEC4;
                    }
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

  uint32_t GetConstantBufferMemberArraySize(const APIRenderState* _pAPIRenderState, PipelineStage _eStage, uint32_t _uIdx, uint32_t _uMemberIdx)
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
                  uint32_t uSize = pDesc->block.members[_uMemberIdx].type_description->traits.array.dims[0];
                  if (uSize == 0u)
                  {
                    uSize = 1;
                  }
                  return uSize;
                }
              }
              break;
            }

            uCurrIdx++;
          }
        }
      }
    }

    THROW_GENERIC_EXCEPTION("[API] Constant buffer member array size invalid ");

    return 0u;
  }

  // Render substate

  APIRenderSubState* CreateAPIRenderSubState(const APIWindow* _pWindow, ResourceFrequency _eFrequency)
  {    
    APIRenderSubState* pSubState = s_oRenderSubStatePool.PullElement();    

    switch (_eFrequency)
    {
    case ResourceFrequency::MATERIAL_INSTANCE:
      pSubState->m_hDecSetLayout = _pWindow->m_hMatInstanceDescSetLayout;
      break;
    case ResourceFrequency::GLOBAL:
      pSubState->m_hDecSetLayout = _pWindow->m_hGlobalDescSetLayout;
      break;
    case ResourceFrequency::RENDER_STEP:
      pSubState->m_hDecSetLayout = _pWindow->m_hStepDescSetLayout;
      break;
    default:
      THROW_GENERIC_EXCEPTION("[API] [VK] Tried to create an APIRenderSubState with invalid ResourceFrequency")
      break;
    }

    uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

    std::vector<VkDescriptorSetLayout> lstLayouts;
    for (int i = 0; i < uNumImages; i++)
    {
      lstLayouts.push_back(pSubState->m_hDecSetLayout);
    }

    VkDescriptorSetAllocateInfo oDescSetAllocInfo{};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = _pWindow->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = lstLayouts.data();

    pSubState->m_pDescSets = s_oVkDescriptorSetPool.PullElements(uNumImages);

    VK_CHECK(vkAllocateDescriptorSets(_pWindow->m_hDevice, &oDescSetAllocInfo, pSubState->m_pDescSets))

    return pSubState;
  }

  void BeginSubStateSetup(const APIWindow* _pWindow, APIRenderSubState* _pSubState, ResourceFrequency _eFrequency)
  {
    /*if (s_oGlobalData.m_pUsingSubState != nullptr)
    {
      THROW_GENERIC_EXCEPTION("[API] EndSubStateSetup was not called before a new BeginSubStateSetup call")
    }*/

    const DescriptorSetLayoutBuilder* pLayoutBuilder = nullptr;

    switch (_eFrequency)
    {
    case ResourceFrequency::MATERIAL_INSTANCE:
      pLayoutBuilder = &_pWindow->m_oMatInstanceLayoutBuilder;
      break;
    case ResourceFrequency::GLOBAL:
      pLayoutBuilder = &_pWindow->m_oGlobalLayoutBuilder;
      break;
    case ResourceFrequency::RENDER_STEP:
      pLayoutBuilder = &_pWindow->m_oStepLayoutBuilder;
      break;
    }

    s_oGlobalData.m_oDescSetUpdater.SetDescriptorSets(_pSubState->m_pDescSets);
    s_oGlobalData.m_oDescSetUpdater.SetLayoutBuilder(pLayoutBuilder);
  }

  void SubStateSetupConstantBuffer(const APIWindow*_pWindow, APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo)
  {       
    uint32_t uBinding = FindBinding(_pWindow, _oBindInfo, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    AddBufferToDescSetUpdater(_pWindow, _pCBuffer, _uSize, uBinding, _oBindInfo.m_uStageFlags);
  }

  void SubStateSetupTexture(const APIWindow* _pWindow, APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
  {     
    uint32_t uBinding = FindBinding(_pWindow, _oBindInfo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    AddImageToDescSetUpdater(_pWindow, _pTexture, uBinding, _oBindInfo.m_uStageFlags);
  }

  void EndSubStateSetup(const APIWindow* _pWindow)
  {        
    s_oGlobalData.m_oDescSetUpdater.Update(_pWindow);

    s_oGlobalData.m_oDescSetUpdater.Clear();
  }

  void BindAPIRenderSubState(const APIWindow* _pWindow, APIRenderState* _pRenderState, APIRenderSubState* _pAPIRenderSubState, ResourceFrequency _eFrequency)
  {        
    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;
    vkCmdBindDescriptorSets(_pWindow->m_pCmdBuffers[uFrameIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, _pRenderState->m_hPipelineLayout, static_cast<uint32_t>(_eFrequency), 1, &_pAPIRenderSubState->m_pDescSets[uFrameIdx], 0, NULL);
  }

  void DestroyRenderSubState(const APIWindow* _pWindow, APIRenderSubState* _pAPIRenderSubState)
  {
    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, 1, &_pWindow->m_pInFlightFences[uFrameIdx], VK_TRUE, UINT64_MAX))

    uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;
    vkFreeDescriptorSets(_pWindow->m_hDevice, _pWindow->m_hDescPool, uNumImages, _pAPIRenderSubState->m_pDescSets);

    s_oVkDescriptorSetPool.ReturnElements(_pAPIRenderSubState->m_pDescSets);

    s_oRenderSubStatePool.ReturnElement(_pAPIRenderSubState);
  }

  // Drawing

  void WaitForLastImage(const APIWindow* _pWindow)
  {
    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, _pWindow->m_uSwapchainImageCount, _pWindow->m_pInFlightFences, VK_TRUE, UINT64_MAX))
    s_oGlobalData.m_bRenderBegan = false;
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
      s_oGlobalData.m_bRenderBegan = false;
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

    s_oGlobalData.m_bRenderBegan = true;

    return 0;
  }

  void BeginDrawOffline(APIWindow* _pWindow)
  {

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, 1, &_pWindow->m_pInFlightFences[uFrameIdx], VK_TRUE, UINT64_MAX))

    VK_CHECK(vkResetFences(_pWindow->m_hDevice, 1, &_pWindow->m_pInFlightFences[uFrameIdx]))

    VK_CHECK(vkResetCommandBuffer(_pWindow->m_pCmdBuffers[uFrameIdx], 0))

    VkCommandBufferBeginInfo oCommandBufferBeginInfo {};
    oCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    oCommandBufferBeginInfo.flags = 0u;
    oCommandBufferBeginInfo.pInheritanceInfo = NULL;

    VK_CHECK(vkBeginCommandBuffer(_pWindow->m_pCmdBuffers[uFrameIdx], &oCommandBufferBeginInfo))
  }

  void DrawMesh(const APIWindow*_pWindow, APIRenderState* _pRenderState, APIMesh* _pMesh, uint32_t _uIndexCount, const void* _pConstantData, uint32_t _uConstantSize)
  {

    VkCommandBuffer& hCmdBuffer = _pWindow->m_pCmdBuffers[_pWindow->m_uCurrFrameIdx];

    VkBuffer vertexBuffers[] = { _pMesh->m_hVertexBuffer };
    VkDeviceSize offsets[] = { 0u };
    vkCmdBindVertexBuffers(hCmdBuffer, 0u, 1u, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(hCmdBuffer, _pMesh->m_hIndexBuffer, 0u, VK_INDEX_TYPE_UINT16);

    vkCmdPushConstants(hCmdBuffer, _pRenderState->m_hPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, _uConstantSize, _pConstantData);

    vkCmdDrawIndexed(hCmdBuffer, _uIndexCount, 1u, 0u, 0u, 0u);

  }

  void EndDraw(APIWindow* _pWindow)
  {    

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;    

    VK_CHECK(vkEndCommandBuffer(_pWindow->m_pCmdBuffers[uFrameIdx]))

    VkSemaphore aWaitSemaphores[] = { _pWindow->m_pImageAvailableSemaphores[uFrameIdx] };
    VkPipelineStageFlags aWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore aSignalSemaphores[] = { _pWindow->m_pRenderFinishedSemaphores[uFrameIdx] };

    VkSubmitInfo oSubmitInfo{};
    oSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    oSubmitInfo.waitSemaphoreCount = s_oGlobalData.m_bRenderBegan ? 1 : 0;
    oSubmitInfo.pWaitSemaphores = aWaitSemaphores;
    oSubmitInfo.pWaitDstStageMask = aWaitStages;
    oSubmitInfo.commandBufferCount = 1u;
    oSubmitInfo.pCommandBuffers = &_pWindow->m_pCmdBuffers[uFrameIdx];
    oSubmitInfo.signalSemaphoreCount = s_oGlobalData.m_bRenderBegan ? 1 : 0;
    oSubmitInfo.pSignalSemaphores = aSignalSemaphores;

    VK_CHECK(vkQueueSubmit(_pWindow->m_hRenderQueue, 1, &oSubmitInfo, _pWindow->m_pInFlightFences[uFrameIdx]))   
  }

  void Present(APIWindow* _pWindow)
  {

    const uint32_t uFrameIdx = _pWindow->m_uCurrFrameIdx;

    VkSemaphore aSignalSemaphores[] = { _pWindow->m_pRenderFinishedSemaphores[uFrameIdx] };

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