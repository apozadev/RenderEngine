#include "Graphics/API/Vulkan/VulkanAPI.h"

#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "Core/Exception.h"

#include "Graphics/Window.h"
#include "Graphics/API/Vulkan/VulkanMacros.h"
#include "Graphics/API/Vulkan/VulkanData.h"
#include "Graphics/API/Vulkan/APIWindow.h"
#include "Graphics/API/Vulkan/APICamera.h"
#include "Graphics/API/Vulkan/APIMesh.h"
#include "Graphics/API/Vulkan/APIConstantBuffer.h"
#include "Graphics/API/Vulkan/APIRenderState.h"
#include "Graphics/API/Vulkan/APIRenderSubState.h"
#include "Graphics/API/Vulkan/APITexture.h"
#include "Graphics/API/Vulkan/VulkanShaderReflection.h"
#include "File/File.h"

namespace api
{
namespace vk
{

  VulkanData s_oGlobalData;

  constexpr uint32_t QUEUE_COUNT = 1u;

  // Forward declarations

  void CreateImage(APIWindow* _pWindow, uint32_t _uWidth, uint32_t _uHeight, VkFormat _eFormat, uint32_t _uMipLevels, VkSampleCountFlagBits _eMsaaSampleCount, VkImageTiling _eTiling, VkImageUsageFlags _uUsage, VkMemoryPropertyFlags _uProperties, VkImage& hImage_, VkDeviceMemory& hMemory_);
  void CreateImageView(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, VkImageAspectFlags _uAspectFlags, VkImageView& hImageView_);
  void TransitionImageLayout(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, VkImageAspectFlags _uAspectFlags, VkImageLayout _eOldLayout, VkImageLayout _eNewLayout);

  ///----------------------------------------------------------------------------------
  //                                Helper functions
  ///----------------------------------------------------------------------------------

  void CreateInstance()
  {

    const char* pDebugLayers[] = { "VK_LAYER_KHRONOS_validation" };


#ifdef _DEBUG
    uint32_t uLayerCount = 1u;
#else
    uint32_t uLayerCount = 0u;
#endif

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo oInstanceCreateInfo = {};
    oInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    oInstanceCreateInfo.pNext = NULL;
    oInstanceCreateInfo.flags = 0u;
    oInstanceCreateInfo.enabledLayerCount = uLayerCount;
    oInstanceCreateInfo.ppEnabledLayerNames = pDebugLayers;
    oInstanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    oInstanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    VkApplicationInfo oApplicationInfo = {};
    oApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    oApplicationInfo.apiVersion = VK_API_VERSION_1_0;
    oApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    oApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    oApplicationInfo.pApplicationName = "App";
    oApplicationInfo.pEngineName = "RenderEngine";

    oInstanceCreateInfo.pApplicationInfo = &oApplicationInfo;

    s_oGlobalData.m_hInstance = VK_NULL_HANDLE;
    VK_CHECK(vkCreateInstance(&oInstanceCreateInfo, NULL, &s_oGlobalData.m_hInstance))
  }

  VkSampleCountFlagBits GetMaxMSAASampleCount()
  {
    VkPhysicalDeviceProperties oDeviceProperties;
    vkGetPhysicalDeviceProperties(s_oGlobalData.m_hPhysicalDevice, &oDeviceProperties);

    VkSampleCountFlags uCounts = oDeviceProperties.limits.framebufferColorSampleCounts & oDeviceProperties.limits.framebufferDepthSampleCounts;
    if (uCounts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (uCounts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (uCounts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (uCounts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (uCounts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (uCounts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;

  }

  void CreatePhysicalDevice()
  {
    uint32_t uPhysicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(s_oGlobalData.m_hInstance, &uPhysicalDeviceCount, NULL))
      VkPhysicalDevice* pPhysicalDevices = new VkPhysicalDevice[uPhysicalDeviceCount];
    VK_CHECK(vkEnumeratePhysicalDevices(s_oGlobalData.m_hInstance, &uPhysicalDeviceCount, pPhysicalDevices))
      s_oGlobalData.m_hPhysicalDevice = VK_NULL_HANDLE;

    bool bCandidateDeviceDiscrete = false;

    constexpr uint32_t uMaxUint = 0xFFFFFFFFu;

    for (int i = 0; i < uPhysicalDeviceCount; i++)
    {
      VkPhysicalDevice hPhysicalDevice = pPhysicalDevices[i];
      VkPhysicalDeviceProperties oDeviceProperties;
      vkGetPhysicalDeviceProperties(hPhysicalDevice, &oDeviceProperties);

      bool bCurrentDeviceDiscrete = oDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

      // Prioritize discrete GPU if found
      if (s_oGlobalData.m_hPhysicalDevice == VK_NULL_HANDLE
        || !bCandidateDeviceDiscrete && bCurrentDeviceDiscrete)
      {

        s_oGlobalData.m_uRenderQueueFamilyIdx = uMaxUint;
        s_oGlobalData.m_uPresentQueueFamilyIdx = uMaxUint;

        uint32_t uQueueFamilyPropertyCount = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(hPhysicalDevice, &uQueueFamilyPropertyCount, NULL);
        VkQueueFamilyProperties* pQueueFamilyProperties = new VkQueueFamilyProperties[uQueueFamilyPropertyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(hPhysicalDevice, &uQueueFamilyPropertyCount, pQueueFamilyProperties);

        // Search for queue families with graphics and presentation capabilities
        for (uint32_t j = 0; j < uQueueFamilyPropertyCount; j++)
        {
          const VkQueueFamilyProperties& rQueueFamilyProperty = pQueueFamilyProperties[j];
          if (s_oGlobalData.m_uRenderQueueFamilyIdx == uMaxUint 
            && (rQueueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
          {
            s_oGlobalData.m_uRenderQueueFamilyIdx = j;
          }
          if (s_oGlobalData.m_uPresentQueueFamilyIdx == uMaxUint
            && (rQueueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
          {
            s_oGlobalData.m_uPresentQueueFamilyIdx = j;
          }
        }

        // If both families were found, we've got a valid candidate
        if (s_oGlobalData.m_uRenderQueueFamilyIdx != uMaxUint
          && s_oGlobalData.m_uPresentQueueFamilyIdx != uMaxUint)
        {
          s_oGlobalData.m_hPhysicalDevice = hPhysicalDevice;
          bCandidateDeviceDiscrete = bCurrentDeviceDiscrete;
        }

        delete[] pQueueFamilyProperties;
      }
    }    

    delete[] pPhysicalDevices;

    if (s_oGlobalData.m_hPhysicalDevice == VK_NULL_HANDLE)
    {
      THROW_GENERIC_EXCEPTION("[API] No device with graphics capabilities was found")
    }

    s_oGlobalData.m_eMaxMsaaSampleCount = GetMaxMSAASampleCount();
  }  

  void CreatePipeline(const file::File& _oVSFile, 
    const file::File& _oPSFile,   
    const RenderStateInfo& _oInfo,
    APIRenderState* _pRenderState_)
  {

    APIWindow* pWindow = _pRenderState_->m_pOwnerWindow;

    // Create shader modules
    
    VkPipelineShaderStageCreateInfo aStages[2];

    // Vertex stage    
        
    VkShaderModuleCreateInfo oVSCreateInfo{};
    oVSCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    oVSCreateInfo.codeSize = _oVSFile.GetSize();
    oVSCreateInfo.pCode = reinterpret_cast<const uint32_t*>(_oVSFile.GetData());

    VkShaderModule hVSShaderModule;
    VK_CHECK(vkCreateShaderModule(pWindow->m_hDevice, &oVSCreateInfo, NULL, &hVSShaderModule))

    aStages[0] = {};
    aStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    aStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    aStages[0].module = hVSShaderModule;
    aStages[0].pName = "main";
          
    // Pixel stage
        
    VkShaderModuleCreateInfo oPSCreateInfo{};
    oPSCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    oPSCreateInfo.codeSize = _oPSFile.GetSize();
    oPSCreateInfo.pCode = reinterpret_cast<const uint32_t*>(_oPSFile.GetData());

    VkShaderModule hPSShaderModule;
    VK_CHECK(vkCreateShaderModule(pWindow->m_hDevice, &oPSCreateInfo, NULL, &hPSShaderModule))

    aStages[1] = {};
    aStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    aStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    aStages[1].module = hPSShaderModule;
    aStages[1].pName = "main";    

    // Vertex input

    auto oBindingDesc = APIMesh::GetBindingDesc();
    auto oAttDesc = APIMesh::GetAttributeDesc();

    VkPipelineVertexInputStateCreateInfo oVertexInputInfo{};
    oVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    oVertexInputInfo.vertexBindingDescriptionCount = 1;
    oVertexInputInfo.pVertexBindingDescriptions = &oBindingDesc;
    oVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(oAttDesc.size());
    oVertexInputInfo.pVertexAttributeDescriptions = oAttDesc.data(); // Optional

    // Input assembly (triangle list)

    VkPipelineInputAssemblyStateCreateInfo oInputAssemblyInfo{};
    oInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    oInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    oInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Tesselation state (unsused)

    VkPipelineTessellationStateCreateInfo oTesselationInfo{};
    oTesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    oTesselationInfo.patchControlPoints = 0u;
    oTesselationInfo.flags = 0u;

    // Dynamic state for viewport and scissors

    VkPipelineViewportStateCreateInfo oViewportState = {};
    oViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    oViewportState.viewportCount = 1;
    oViewportState.pViewports = nullptr; // Set to nullptr for dynamic state
    oViewportState.scissorCount = 1;
    oViewportState.pScissors = nullptr;  // Set to nullptr for dynamic state

    VkPipelineDynamicStateCreateInfo oDynamicState = {};
    oDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    oDynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    oDynamicState.pDynamicStates = dynamicStates.data();

    // Rasterizer

    VkPipelineRasterizationStateCreateInfo oRasterizer{};
    oRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    oRasterizer.depthClampEnable = VK_FALSE;
    oRasterizer.rasterizerDiscardEnable = VK_FALSE;
    oRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    oRasterizer.lineWidth = 1.0f;
    oRasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    oRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    oRasterizer.depthBiasEnable = VK_FALSE;
    oRasterizer.depthBiasConstantFactor = 0.0f; // Optional
    oRasterizer.depthBiasClamp = 0.0f; // Optional
    oRasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    // Multisampling (disabled)

    VkPipelineMultisampleStateCreateInfo oMultisampling{};
    oMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    oMultisampling.sampleShadingEnable = VK_FALSE;
    oMultisampling.rasterizationSamples = s_oGlobalData.m_eMaxMsaaSampleCount;
    oMultisampling.minSampleShading = 1.0f; // Optional
    oMultisampling.pSampleMask = NULL; // Optional
    oMultisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    oMultisampling.alphaToOneEnable = VK_FALSE; // Optional

    // Depth stencil 

    VkPipelineDepthStencilStateCreateInfo oDepthStencil {};
    oDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    oDepthStencil.depthTestEnable = VK_TRUE;
    oDepthStencil.depthWriteEnable = VK_TRUE;
    oDepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    oDepthStencil.depthBoundsTestEnable = VK_FALSE;
    oDepthStencil.stencilTestEnable = VK_FALSE;
    oDepthStencil.minDepthBounds = 0.f;
    oDepthStencil.maxDepthBounds = 1.f;

    // Blending (disabled)

    VkPipelineColorBlendAttachmentState oBlendAttachment {};
    oBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    oBlendAttachment.blendEnable = VK_FALSE;
    oBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    oBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    oBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    oBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    oBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    oBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo oColorBlending {};
    oColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    oColorBlending.logicOpEnable = VK_FALSE;
    oColorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    oColorBlending.attachmentCount = 1;
    oColorBlending.pAttachments = &oBlendAttachment;
    oColorBlending.blendConstants[0] = 0.0f; // Optional
    oColorBlending.blendConstants[1] = 0.0f; // Optional
    oColorBlending.blendConstants[2] = 0.0f; // Optional
    oColorBlending.blendConstants[3] = 0.0f; // Optional

    // Pipeline layout (from desc sets)    

    VkDescriptorSetLayout aSetLayouts[3] =
    {
      pWindow->m_hGlobalDescSetLayout
      , _pRenderState_->m_hDescSetLayout
      , _pRenderState_->m_hSubDescSetLayout
    };

    // Push constants
    
    VkPushConstantRange push_constant;
    push_constant.offset = 0;    
    push_constant.size = _oInfo.m_uMeshConstantSize;    
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo oPipelineLayoutCreateInfo {};
    oPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    oPipelineLayoutCreateInfo.setLayoutCount = 3u;
    oPipelineLayoutCreateInfo.pSetLayouts = aSetLayouts;
    oPipelineLayoutCreateInfo.pushConstantRangeCount = 1u;
    oPipelineLayoutCreateInfo.pPushConstantRanges = &push_constant;

    VK_CHECK(vkCreatePipelineLayout(pWindow->m_hDevice, &oPipelineLayoutCreateInfo, NULL, &_pRenderState_->m_hPipelineLayout))

    // Create Pipeline object

    VkGraphicsPipelineCreateInfo oPipelineInfo {};
    oPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    oPipelineInfo.stageCount = 2;
    oPipelineInfo.pStages = aStages;
    oPipelineInfo.pVertexInputState = &oVertexInputInfo;
    oPipelineInfo.pInputAssemblyState = &oInputAssemblyInfo;
    oPipelineInfo.pTessellationState = &oTesselationInfo;
    oPipelineInfo.pViewportState = &oViewportState;
    oPipelineInfo.pRasterizationState = &oRasterizer;
    oPipelineInfo.pMultisampleState = &oMultisampling;
    oPipelineInfo.pDepthStencilState = &oDepthStencil; // Optional
    oPipelineInfo.pColorBlendState = &oColorBlending;
    oPipelineInfo.pDynamicState = NULL;
    oPipelineInfo.layout = _pRenderState_->m_hPipelineLayout;
    oPipelineInfo.renderPass = pWindow->m_hRenderPass;
    oPipelineInfo.subpass = 0u;
    oPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    oPipelineInfo.basePipelineIndex = -1; // Optional
    oPipelineInfo.pDynamicState = &oDynamicState;

    VK_CHECK(vkCreateGraphicsPipelines(pWindow->m_hDevice, VK_NULL_HANDLE, 1u, &oPipelineInfo, NULL, &_pRenderState_->m_hGraphicsPipeline))

    vkDestroyShaderModule(pWindow->m_hDevice, hVSShaderModule, NULL);
    vkDestroyShaderModule(pWindow->m_hDevice, hPSShaderModule, NULL);
  }

  void CreateLogicalDevice(APIWindow* _pWindow)
  {    

    const char* aDeviceExtensions [] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    uint32_t uDeviceExtensionCount = 1u;


    VkDeviceQueueCreateInfo aDeviceQueueCreateInfos[2];

    unsigned int uQueueInfoCount;

    if (s_oGlobalData.m_uRenderQueueFamilyIdx == s_oGlobalData.m_uPresentQueueFamilyIdx)
    {
      uQueueInfoCount = 1;

      static const float queuePriorities[] = { 1.0f, 1.0f};

      aDeviceQueueCreateInfos[0] = {};
      aDeviceQueueCreateInfos[0].flags = 0u;
      aDeviceQueueCreateInfos[0].pNext = NULL;
      aDeviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      aDeviceQueueCreateInfos[0].queueCount = 2;
      aDeviceQueueCreateInfos[0].queueFamilyIndex = s_oGlobalData.m_uRenderQueueFamilyIdx;
      aDeviceQueueCreateInfos[0].pQueuePriorities = queuePriorities;
    }
    else
    {
      uQueueInfoCount = 2;

      static const float queuePriorities[] = { 1.0f };

      aDeviceQueueCreateInfos[0] = {};
      aDeviceQueueCreateInfos[0].flags = 0u;
      aDeviceQueueCreateInfos[0].pNext = NULL;
      aDeviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      aDeviceQueueCreateInfos[0].queueCount = 1;
      aDeviceQueueCreateInfos[0].queueFamilyIndex = s_oGlobalData.m_uRenderQueueFamilyIdx;
      aDeviceQueueCreateInfos[0].pQueuePriorities = queuePriorities;

      aDeviceQueueCreateInfos[1] = {};
      aDeviceQueueCreateInfos[1].flags = 0u;
      aDeviceQueueCreateInfos[1].pNext = NULL;
      aDeviceQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      aDeviceQueueCreateInfos[1].queueCount = 1;
      aDeviceQueueCreateInfos[1].queueFamilyIndex = s_oGlobalData.m_uPresentQueueFamilyIdx;
      aDeviceQueueCreateInfos[1].pQueuePriorities = queuePriorities;
    }

    VkPhysicalDeviceFeatures oDeviceFeatures{};
    vkGetPhysicalDeviceFeatures(s_oGlobalData.m_hPhysicalDevice, &oDeviceFeatures);

    VkDeviceCreateInfo oDeviceCreateInfo = {};
    oDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    oDeviceCreateInfo.queueCreateInfoCount = uQueueInfoCount;
    oDeviceCreateInfo.pQueueCreateInfos = &aDeviceQueueCreateInfos[0];
    oDeviceCreateInfo.ppEnabledLayerNames = NULL;
    oDeviceCreateInfo.enabledLayerCount = 0u;
    oDeviceCreateInfo.enabledExtensionCount = uDeviceExtensionCount;
    oDeviceCreateInfo.ppEnabledExtensionNames = aDeviceExtensions;   
    oDeviceCreateInfo.pEnabledFeatures = &oDeviceFeatures;

    VK_CHECK(vkCreateDevice(s_oGlobalData.m_hPhysicalDevice, &oDeviceCreateInfo, NULL, &_pWindow->m_hDevice))

  }

  void RetrieveQueues(APIWindow* _pWindow)
  {
    _pWindow->m_hRenderQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(_pWindow->m_hDevice, s_oGlobalData.m_uRenderQueueFamilyIdx, 0, &_pWindow->m_hRenderQueue);

    if (_pWindow->m_hRenderQueue == VK_NULL_HANDLE)
    {
      THROW_GENERIC_EXCEPTION("[API] Could not get render queue. WTF Vulkan??")
    }

    const unsigned int uPresentQueueIdx = s_oGlobalData.m_uPresentQueueFamilyIdx == s_oGlobalData.m_uRenderQueueFamilyIdx ? 1 : 0;
    _pWindow->m_hPresentQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(_pWindow->m_hDevice, s_oGlobalData.m_uPresentQueueFamilyIdx, uPresentQueueIdx, &_pWindow->m_hPresentQueue);

    if (_pWindow->m_hPresentQueue == VK_NULL_HANDLE)
    {
      THROW_GENERIC_EXCEPTION("[API] Could not get presentation queue. WTF Vulkan??")
    }
  }

  void CreateSurface(APIWindow* _pWindow, GLFWwindow* _pGlfwWindow)
  {
    VK_CHECK(glfwCreateWindowSurface(s_oGlobalData.m_hInstance, _pGlfwWindow, NULL, &_pWindow->m_hSurface))

      VkBool32 bPresentSupported;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(s_oGlobalData.m_hPhysicalDevice, s_oGlobalData.m_uPresentQueueFamilyIdx, _pWindow->m_hSurface, &bPresentSupported))

      if (bPresentSupported == VK_FALSE)
      {
        THROW_GENERIC_EXCEPTION("[API] Device does not support presentation for window surface");
      }

    VkSurfaceCapabilitiesKHR oSurfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &oSurfaceCapabilities))

      // Get swapchain number of images

      _pWindow->m_uSwapchainImageCount = oSurfaceCapabilities.minImageCount;
    if (oSurfaceCapabilities.maxImageCount != 0u || _pWindow->m_uSwapchainImageCount + 1u <= oSurfaceCapabilities.maxImageCount)
    {
      _pWindow->m_uSwapchainImageCount += 1u;
    }
  }

  void CreateColorBuffer(APIWindow* _pWindow)
  {
    CreateImage(_pWindow, _pWindow->m_oExtent.width, _pWindow->m_oExtent.height, _pWindow->m_eSwapchainFormat, 1u, s_oGlobalData.m_eMaxMsaaSampleCount,
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      _pWindow->m_hColorImage, _pWindow->m_hColorImageMemory);

    CreateImageView(_pWindow, _pWindow->m_hColorImage, _pWindow->m_eSwapchainFormat, 1u, VK_IMAGE_ASPECT_COLOR_BIT, _pWindow->m_hColorImageView);
  }

  void CreateDepthBuffer(APIWindow*_pWindow)
  {

    VkFormat eFormat = VK_FORMAT_D32_SFLOAT;
    VkImageAspectFlags uAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

    CreateImage(_pWindow, 
      _pWindow->m_oExtent.width, 
      _pWindow->m_oExtent.height,
      eFormat,
      1u,
      s_oGlobalData.m_eMaxMsaaSampleCount,
      VK_IMAGE_TILING_OPTIMAL, 
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
      _pWindow->m_hDepthImage,
      _pWindow->m_hDepthImageMemory);

    CreateImageView(_pWindow, _pWindow->m_hDepthImage, eFormat, 1u, uAspectFlags, _pWindow->m_hDepthImageView);

    TransitionImageLayout(_pWindow, _pWindow->m_hDepthImage, eFormat, 1u, uAspectFlags, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }

  void CreateSwapchain(APIWindow* _pWindow)
  {
    // Choose from supported formats for swapchain    

    {
      uint32_t uSurfaceFormatCount = 0;
      VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &uSurfaceFormatCount, NULL))

      VkSurfaceFormatKHR* pSurfaceFormats = new VkSurfaceFormatKHR[uSurfaceFormatCount];
      VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &uSurfaceFormatCount, pSurfaceFormats))

      if (uSurfaceFormatCount == 1 && pSurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
      {
        _pWindow->m_eSwapchainFormat = VK_FORMAT_R8G8B8A8_UNORM;
      }
      else
      {
        _pWindow->m_eSwapchainFormat = pSurfaceFormats[0].format;
      }

      _pWindow->m_eSwapchainColorSpace = pSurfaceFormats[0].colorSpace;

      _pWindow->m_eSwapchainFormat = pSurfaceFormats[0].format;
      for (int i = 0; i < uSurfaceFormatCount; i++)
      {
        if (pSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB
          && pSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
          _pWindow->m_eSwapchainFormat = pSurfaceFormats[i].format;
          _pWindow->m_eSwapchainColorSpace = pSurfaceFormats[i].colorSpace;
          break;
        }
      }

      delete[] pSurfaceFormats;
    }

    // Choose transform mode for swapchain

    VkSurfaceCapabilitiesKHR oSurfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &oSurfaceCapabilities))    

    // Choose present mode for swapchain

    VkPresentModeKHR ePresentMode = VK_PRESENT_MODE_FIFO_KHR;
    {
      uint32_t uPresentModeCount = 0;
      VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &uPresentModeCount, NULL))
        VkPresentModeKHR* pPresentModes = new VkPresentModeKHR[uPresentModeCount];
      VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &uPresentModeCount, pPresentModes))

        for (int i = 0; i < uPresentModeCount; i++)
        {
          if (pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
          {
            ePresentMode = pPresentModes[i];
            break;
          }
        }

      delete[] pPresentModes;
    }


    // Create swapchain

    VkSwapchainCreateInfoKHR oSwapchainCreateInfo = {};
    oSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    oSwapchainCreateInfo.surface = _pWindow->m_hSurface;
    oSwapchainCreateInfo.clipped = VK_TRUE;
    oSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    oSwapchainCreateInfo.flags = 0u;
    oSwapchainCreateInfo.imageArrayLayers = 1u;
    oSwapchainCreateInfo.imageColorSpace = _pWindow->m_eSwapchainColorSpace;
    oSwapchainCreateInfo.imageExtent = oSurfaceCapabilities.currentExtent;
    oSwapchainCreateInfo.imageFormat = _pWindow->m_eSwapchainFormat;
    oSwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    oSwapchainCreateInfo.minImageCount = _pWindow->m_uSwapchainImageCount;
    oSwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    oSwapchainCreateInfo.pNext = VK_NULL_HANDLE;
    oSwapchainCreateInfo.presentMode = ePresentMode;
    oSwapchainCreateInfo.preTransform = oSurfaceCapabilities.currentTransform;

    /*if (s_oGlobalData.m_uRenderQueueFamilyIdx == s_oGlobalData.m_uPresentQueueFamilyIdx)
    {
      uint32_t aQueueFamilyIndices[] = { s_oGlobalData.m_uRenderQueueFamilyIdx, s_oGlobalData.m_uPresentQueueFamilyIdx };

      oSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      oSwapchainCreateInfo.queueFamilyIndexCount = 2u;
      oSwapchainCreateInfo.pQueueFamilyIndices = aQueueFamilyIndices;
    }
    else
    {

      uint32_t aQueueFamilyIndices[] = { s_oGlobalData.m_uRenderQueueFamilyIdx};

      oSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      oSwapchainCreateInfo.queueFamilyIndexCount = 1u;
      oSwapchainCreateInfo.pQueueFamilyIndices = aQueueFamilyIndices;
    }*/    

    uint32_t aQueueFamilyIndices[] = { s_oGlobalData.m_uPresentQueueFamilyIdx };

    oSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    oSwapchainCreateInfo.queueFamilyIndexCount = 0u;
    oSwapchainCreateInfo.pQueueFamilyIndices = NULL;


    VK_CHECK(vkCreateSwapchainKHR(_pWindow->m_hDevice, &oSwapchainCreateInfo, NULL, &_pWindow->m_hSwapchain))

    _pWindow->m_oExtent = oSurfaceCapabilities.currentExtent;

    // Get swapchain images

    _pWindow->m_uSwapchainImageCount = 0u;
    VK_CHECK(vkGetSwapchainImagesKHR(_pWindow->m_hDevice, _pWindow->m_hSwapchain, &_pWindow->m_uSwapchainImageCount, NULL))
    _pWindow->m_pSwapchainImages = new VkImage[_pWindow->m_uSwapchainImageCount];
    VK_CHECK(vkGetSwapchainImagesKHR(_pWindow->m_hDevice, _pWindow->m_hSwapchain, &_pWindow->m_uSwapchainImageCount, _pWindow->m_pSwapchainImages))

    // Create views for swapchain images

    _pWindow->m_pSwapChainImageViews = new VkImageView[_pWindow->m_uSwapchainImageCount];
    for (int i = 0; i < _pWindow->m_uSwapchainImageCount; i++)
    {
      VkImageViewCreateInfo oCreateInfo = {};
      oCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      oCreateInfo.pNext = NULL;
      oCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      oCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      oCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      oCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      oCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      oCreateInfo.subresourceRange.levelCount = 1u;
      oCreateInfo.subresourceRange.baseMipLevel = 0u;
      oCreateInfo.subresourceRange.layerCount = 1u;
      oCreateInfo.subresourceRange.baseArrayLayer = 0u;
      oCreateInfo.image = _pWindow->m_pSwapchainImages[i];
      oCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      oCreateInfo.format = _pWindow->m_eSwapchainFormat;

      VK_CHECK(vkCreateImageView(_pWindow->m_hDevice, &oCreateInfo, NULL, &_pWindow->m_pSwapChainImageViews[i]))
    }
  }  

  void CreateRenderPass(APIWindow* _pWindow)
  {
    // Render pass (to backbuffer)

    VkAttachmentDescription oColorAttachmentDesc{};
    oColorAttachmentDesc.format = _pWindow->m_eSwapchainFormat;
    oColorAttachmentDesc.samples = s_oGlobalData.m_eMaxMsaaSampleCount;
    oColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    oColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    oColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    oColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    oColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    oColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription oDepthAttachmentDesc{};
    oDepthAttachmentDesc.format = VK_FORMAT_D32_SFLOAT;;
    oDepthAttachmentDesc.samples = s_oGlobalData.m_eMaxMsaaSampleCount;
    oDepthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    oDepthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    oDepthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    oDepthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    oDepthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    oDepthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription oColorResolveAttachmentDesc{};
    oColorResolveAttachmentDesc.format = oColorAttachmentDesc.format;
    oColorResolveAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    oColorResolveAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    oColorResolveAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    oColorResolveAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    oColorResolveAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    oColorResolveAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    oColorResolveAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference oColorAttachmentRef{};
    oColorAttachmentRef.attachment = 0u;
    oColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference oDepthAttachmentRef{};
    oDepthAttachmentRef.attachment = 1u;
    oDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference oColorResolveAttachmentRef{};
    oColorResolveAttachmentRef.attachment = 2u;
    oColorResolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription oSubpassDesc{};
    oSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    oSubpassDesc.colorAttachmentCount = 1u;
    oSubpassDesc.pColorAttachments = &oColorAttachmentRef;    
    oSubpassDesc.pDepthStencilAttachment = &oDepthAttachmentRef;
    oSubpassDesc.pResolveAttachments = &oColorResolveAttachmentRef;

    VkSubpassDependency oSubpassDependency{};
    oSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    oSubpassDependency.dstSubpass = 0u;
    oSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    oSubpassDependency.srcAccessMask = 0;
    oSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    oSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


    constexpr uint32_t uAttachmentCount = 3u;
    VkAttachmentDescription aAttachments[uAttachmentCount]{ oColorAttachmentDesc, oDepthAttachmentDesc, oColorResolveAttachmentDesc };

    VkRenderPassCreateInfo oRenderPassCreateInfo{};
    oRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    oRenderPassCreateInfo.attachmentCount = uAttachmentCount;
    oRenderPassCreateInfo.pAttachments = aAttachments;
    oRenderPassCreateInfo.subpassCount = 1u;
    oRenderPassCreateInfo.pSubpasses = &oSubpassDesc;
    oRenderPassCreateInfo.dependencyCount = 1u;
    oRenderPassCreateInfo.pDependencies = &oSubpassDependency;

    VK_CHECK(vkCreateRenderPass(_pWindow->m_hDevice, &oRenderPassCreateInfo, NULL, &_pWindow->m_hRenderPass))
  }

  void CreateFramebuffers(APIWindow* _pWindow)
  {    
    _pWindow->m_pFramebuffers = new VkFramebuffer[_pWindow->m_uSwapchainImageCount];

    for (int i = 0; i < _pWindow->m_uSwapchainImageCount; i++)
    {
      VkImageView pAttachments[] = { _pWindow->m_hColorImageView, _pWindow->m_hDepthImageView, _pWindow->m_pSwapChainImageViews[i] };

      VkFramebufferCreateInfo oFramebufferInfo = {};
      oFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      oFramebufferInfo.attachmentCount = 3u;
      oFramebufferInfo.pAttachments = pAttachments;
      oFramebufferInfo.renderPass = _pWindow->m_hRenderPass;
      oFramebufferInfo.width = _pWindow->m_oExtent.width;
      oFramebufferInfo.height = _pWindow->m_oExtent.height;
      oFramebufferInfo.layers = 1;

      VK_CHECK(vkCreateFramebuffer(_pWindow->m_hDevice, &oFramebufferInfo, NULL, &_pWindow->m_pFramebuffers[i]))

    }
  }
    
  void CreateCommandBuffers(APIWindow* _pWindow)
  {     
    {
      VkCommandPoolCreateInfo oCmdPoolCreateInfo = {};
      oCmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      oCmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      oCmdPoolCreateInfo.queueFamilyIndex = s_oGlobalData.m_uRenderQueueFamilyIdx;
      oCmdPoolCreateInfo.pNext = NULL;
      VK_CHECK(vkCreateCommandPool(_pWindow->m_hDevice, &oCmdPoolCreateInfo, NULL, &_pWindow->m_hRenderCmdPool))
    }
    // If render and present queue families are the same, share command pool
    if (s_oGlobalData.m_uRenderQueueFamilyIdx == s_oGlobalData.m_uPresentQueueFamilyIdx)
    {
      _pWindow->m_hPresentCmdPool = _pWindow->m_hRenderCmdPool;
    }
    else
    {
      VkCommandPoolCreateInfo oCmdPoolCreateInfo = {};
      oCmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      oCmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      oCmdPoolCreateInfo.queueFamilyIndex = s_oGlobalData.m_uPresentQueueFamilyIdx;
      oCmdPoolCreateInfo.pNext = NULL;
      VK_CHECK(vkCreateCommandPool(_pWindow->m_hDevice, &oCmdPoolCreateInfo, NULL, &_pWindow->m_hPresentCmdPool))
    }

    // Create render command buffer
    {      
      VkCommandBufferAllocateInfo oCmdBufferAllocInfo = {};
      oCmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      oCmdBufferAllocInfo.commandPool = _pWindow->m_hRenderCmdPool;
      oCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      oCmdBufferAllocInfo.commandBufferCount = 1u;
      VK_CHECK(vkAllocateCommandBuffers(_pWindow->m_hDevice, &oCmdBufferAllocInfo, &_pWindow->m_hRenderCmdBuffer))
    }

    // Create present command buffer
    /*{
      VkCommandBufferAllocateInfo oCmdBufferAllocInfo = {};
      oCmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      oCmdBufferAllocInfo.commandPool = _pWindow->m_hRenderCmdPool;
      oCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      oCmdBufferAllocInfo.commandBufferCount = 1u;
      VK_CHECK(vkAllocateCommandBuffers(_pWindow->m_hDevice, &oCmdBufferAllocInfo, &_pWindow->m_hRenderCmdBuffer))
    }*/

    // Create fences for command buffers
    //{
    //  _pWindow->m_uFenceCount = _pWindow->m_uCmdBufferCount - 1u;
    //  _pWindow->m_pFences = new VkFence[pWindow->m_uFenceCount];
    //  for (int i = 0; i < pWindow->m_uFenceCount; ++i)
    //  {
    //    VkFenceCreateInfo fenceCreateInfo = {};
    //    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //    // We need this so we can wait for them on the first try
    //    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    //    VK_CHECK(vkCreateFence(pWindow->m_hDevice, &fenceCreateInfo, NULL, &pWindow->m_pFences[i]))
    //  }
    //}
  }  

  void CreateSyncObjects(APIWindow* _pWindow)
  {
    VkSemaphoreCreateInfo oSemaphoreInfo{};
    oSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK(vkCreateSemaphore(_pWindow->m_hDevice, &oSemaphoreInfo, NULL, &_pWindow->m_hImageAvailableSemaphore))
    VK_CHECK(vkCreateSemaphore(_pWindow->m_hDevice, &oSemaphoreInfo, NULL, &_pWindow->m_hRenderFinishedSemaphore))

    VkFenceCreateInfo oFenceInfo{};
    oFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    oFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(_pWindow->m_hDevice, &oFenceInfo, NULL, &_pWindow->m_hInFlightFence))
  }

  void CreateDescriptorPool(APIWindow* _pWindow)
  {

    VkDescriptorPoolSize aPoolSizes[2];
    
    aPoolSizes[0].descriptorCount = 999u;
    aPoolSizes[0].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    aPoolSizes[1].descriptorCount = 999u;
    aPoolSizes[1].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorPoolCreateInfo oPoolInfo{};
    oPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    oPoolInfo.flags = 0u;
    oPoolInfo.maxSets = 999u;
    oPoolInfo.poolSizeCount = 2u;
    oPoolInfo.pPoolSizes = aPoolSizes;

    VK_CHECK(vkCreateDescriptorPool(_pWindow->m_hDevice, &oPoolInfo, NULL, &_pWindow->m_hDescPool))
  }

  void CreateGlobalDescriptorLayout(APIWindow* _pWindow)
  {    
    VkDescriptorSetLayoutBinding oBinding{};
    oBinding.binding = 0;
    oBinding.descriptorCount = 1u;
    oBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    oBinding.pImmutableSamplers = NULL;

    s_oGlobalData.m_oGlobalLayoutBuilder.AddLayoutBinding(std::move(oBinding));

    _pWindow->m_hGlobalDescSetLayout = s_oGlobalData.m_oGlobalLayoutBuilder.Build(_pWindow->m_hDevice);
    
  }

  void DestroySwapchain(APIWindow* _pWindow)
  {

    vkDestroyImageView(_pWindow->m_hDevice, _pWindow->m_hColorImageView, nullptr);
    vkDestroyImage(_pWindow->m_hDevice, _pWindow->m_hColorImage, nullptr);
    vkFreeMemory(_pWindow->m_hDevice, _pWindow->m_hColorImageMemory, nullptr);

    vkDestroyImageView(_pWindow->m_hDevice, _pWindow->m_hDepthImageView, nullptr);
    vkDestroyImage(_pWindow->m_hDevice, _pWindow->m_hDepthImage, nullptr);
    vkFreeMemory(_pWindow->m_hDevice, _pWindow->m_hDepthImageMemory, nullptr);

    for (int i = 0; i < _pWindow->m_uSwapchainImageCount; i++)
    {
      vkDestroyFramebuffer(_pWindow->m_hDevice, _pWindow->m_pFramebuffers[i], NULL);
      vkDestroyImageView(_pWindow->m_hDevice, _pWindow->m_pSwapChainImageViews[i], NULL);
    }

    vkDestroySwapchainKHR(_pWindow->m_hDevice, _pWindow->m_hSwapchain, NULL);

    delete[] _pWindow->m_pFramebuffers;
    delete[] _pWindow->m_pSwapchainImages;
    delete[] _pWindow->m_pSwapChainImageViews;
  }

  void RecreateSwapchain(APIWindow* _pWindow)
  {    
    vkDeviceWaitIdle(_pWindow->m_hDevice);
    DestroySwapchain(_pWindow);
    CreateSwapchain(_pWindow);   
    CreateColorBuffer(_pWindow);
    CreateDepthBuffer(_pWindow);
    CreateFramebuffers(_pWindow);
  }

  uint32_t FindMemoryType(uint32_t _uTypeFilter, VkMemoryPropertyFlags _uProperties)
  {
    VkPhysicalDeviceMemoryProperties oMemProperties;
    vkGetPhysicalDeviceMemoryProperties(s_oGlobalData.m_hPhysicalDevice, &oMemProperties);

    for (uint32_t i = 0; i < oMemProperties.memoryTypeCount; i++)
    {
      if ((_uTypeFilter & (1 << i))
        && (oMemProperties.memoryTypes[i].propertyFlags & _uProperties) == _uProperties)
      {
        return i;
      }
    }

    THROW_GENERIC_EXCEPTION("Failed to find suitable memory type")
  }

  void CreateBuffer(APIWindow* _pWindow, size_t _uSize, VkBufferUsageFlags _uUsage, VkMemoryPropertyFlags _uProperties, VkBuffer& hBuffer_, VkDeviceMemory& hDeviceMemory_)
  {

    VkDevice hDevice = _pWindow->m_hDevice;

    VkBufferCreateInfo oBufferInfo{};
    oBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    oBufferInfo.size = _uSize;
    oBufferInfo.usage = _uUsage;
    oBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    oBufferInfo.flags = 0u;

    VK_CHECK(vkCreateBuffer(hDevice, &oBufferInfo, NULL, &hBuffer_))

    VkMemoryRequirements oMemRequirements;
    vkGetBufferMemoryRequirements(hDevice, hBuffer_, &oMemRequirements);

    VkMemoryAllocateInfo oAllocInfo{};
    oAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    oAllocInfo.allocationSize = oMemRequirements.size;
    oAllocInfo.memoryTypeIndex = FindMemoryType(oMemRequirements.memoryTypeBits, _uProperties);

    VK_CHECK(vkAllocateMemory(hDevice, &oAllocInfo, NULL, &hDeviceMemory_));

    VK_CHECK(vkBindBufferMemory(hDevice, hBuffer_, hDeviceMemory_, 0))    
  }

  void CreateImage(APIWindow* _pWindow, uint32_t _uWidth, uint32_t _uHeight, VkFormat _eFormat, uint32_t _uMipLevels, VkSampleCountFlagBits _eMsaaSampleCount, VkImageTiling _eTiling, VkImageUsageFlags _uUsage, VkMemoryPropertyFlags _uProperties, VkImage& hImage_, VkDeviceMemory& hMemory_)
  {
    VkImageCreateInfo oImageCreateInfo{};

    oImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    oImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    oImageCreateInfo.extent.width = _uWidth;
    oImageCreateInfo.extent.height = _uHeight;
    oImageCreateInfo.extent.depth = 1u;
    oImageCreateInfo.mipLevels = _uMipLevels;
    oImageCreateInfo.arrayLayers = 1;
    oImageCreateInfo.format = _eFormat;
    oImageCreateInfo.tiling = _eTiling;
    oImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    oImageCreateInfo.usage = _uUsage;
    oImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    oImageCreateInfo.samples = _eMsaaSampleCount;
    oImageCreateInfo.flags = 0u;    

    VK_CHECK(vkCreateImage(_pWindow->m_hDevice, &oImageCreateInfo, NULL, &hImage_))

    VkMemoryRequirements oMemRequirements;
    vkGetImageMemoryRequirements(_pWindow->m_hDevice, hImage_, &oMemRequirements);

    VkMemoryAllocateInfo oAllocInfo{};
    oAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    oAllocInfo.allocationSize = oMemRequirements.size;
    oAllocInfo.memoryTypeIndex = FindMemoryType(oMemRequirements.memoryTypeBits, _uProperties);

    VK_CHECK(vkAllocateMemory(_pWindow->m_hDevice, &oAllocInfo, NULL, &hMemory_))

    VK_CHECK(vkBindImageMemory(_pWindow->m_hDevice, hImage_, hMemory_, 0))
  }  

  void CreateImageView(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, VkImageAspectFlags _uAspectFlags, VkImageView& hImageView_)
  {
    VkImageViewCreateInfo oCreateInfo = {};
    oCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    oCreateInfo.pNext = NULL;
    oCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    oCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    oCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    oCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    oCreateInfo.subresourceRange.aspectMask = _uAspectFlags;    
    oCreateInfo.subresourceRange.baseMipLevel = 0u;
    oCreateInfo.subresourceRange.levelCount = _uMipLevels;
    oCreateInfo.subresourceRange.baseArrayLayer = 0u;
    oCreateInfo.subresourceRange.layerCount = 1u;    
    oCreateInfo.image = _hImage;
    oCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    oCreateInfo.format = _eFormat;

    VK_CHECK(vkCreateImageView(_pWindow->m_hDevice, &oCreateInfo, NULL, &hImageView_))
  }

  void CreateTextureSampler(APIWindow* _pWindow, APITexture* _pTexture, uint32_t _uMipLevels)
  {

    VkPhysicalDeviceProperties oProperties{};
    vkGetPhysicalDeviceProperties(s_oGlobalData.m_hPhysicalDevice, &oProperties);

    VkSamplerCreateInfo oSamplerInfo{};
    oSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    oSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    oSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    oSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    oSamplerInfo.anisotropyEnable = VK_TRUE;
    oSamplerInfo.maxAnisotropy = oProperties.limits.maxSamplerAnisotropy;
    oSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    oSamplerInfo.unnormalizedCoordinates = VK_FALSE;
    oSamplerInfo.compareEnable = VK_FALSE;
    oSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    oSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;    
    oSamplerInfo.minLod = 0.0f;
    oSamplerInfo.maxLod = static_cast<float>(_uMipLevels);
    oSamplerInfo.mipLodBias = 0.0f;

    VK_CHECK(vkCreateSampler(_pWindow->m_hDevice, &oSamplerInfo, NULL, &_pTexture->m_hSampler))
  }

  void DestroyBuffer(APIWindow* _pWindow, VkBuffer _hBuffer, VkDeviceMemory _hDeviceMemory_)
  {
    vkDestroyBuffer(_pWindow->m_hDevice, _hBuffer, NULL);
    vkFreeMemory(_pWindow->m_hDevice, _hDeviceMemory_, NULL);
  }

  void SetBufferData(APIWindow* _pWindow, const void* _pData, size_t _uSize, VkDeviceMemory& hDeviceMemory_)
  {
    VkDevice hDevice = _pWindow->m_hDevice;
    void* pDeviceData;
    VK_CHECK(vkMapMemory(hDevice, hDeviceMemory_, 0, _uSize, 0, &pDeviceData))
    memcpy(pDeviceData, _pData, _uSize);
    vkUnmapMemory(hDevice, hDeviceMemory_);
  }

  VkCommandBuffer BeginTempCmdBuffer(APIWindow* _pWindow)
  {

    VkCommandBufferAllocateInfo oAllocInfo{};
    oAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    oAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    oAllocInfo.commandPool = _pWindow->m_hRenderCmdPool;
    oAllocInfo.commandBufferCount = 1;

    VkCommandBuffer hCommandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(_pWindow->m_hDevice, &oAllocInfo, &hCommandBuffer))

    VkCommandBufferBeginInfo oBeginInfo
    {
    };
    oBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    oBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(hCommandBuffer, &oBeginInfo))

    return hCommandBuffer;
  }

  void EndTempCmdBuffer(APIWindow* _pWindow, VkCommandBuffer _hCmdBuffer)
  {
    VK_CHECK(vkEndCommandBuffer(_hCmdBuffer))

      VkSubmitInfo oSubmitInfo
    {
    };
    oSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    oSubmitInfo.commandBufferCount = 1;
    oSubmitInfo.pCommandBuffers = &_hCmdBuffer;

    VK_CHECK(vkQueueSubmit(_pWindow->m_hRenderQueue, 1, &oSubmitInfo, VK_NULL_HANDLE))
    VK_CHECK(vkQueueWaitIdle(_pWindow->m_hRenderQueue))

    vkFreeCommandBuffers(_pWindow->m_hDevice, _pWindow->m_hRenderCmdPool, 1u, &_hCmdBuffer);
  }

  void CopyBuffer(APIWindow* _pWindow, VkBuffer _hSrcBuffer, VkBuffer _hDstBuffer, VkDeviceSize _uSize)
  {
    VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);

    VkBufferCopy oCopyRegion{};
    oCopyRegion.srcOffset = 0; // Optional
    oCopyRegion.dstOffset = 0; // Optional
    oCopyRegion.size = _uSize;
    vkCmdCopyBuffer(hCmdBuffer, _hSrcBuffer, _hDstBuffer, 1, &oCopyRegion);

    EndTempCmdBuffer(_pWindow, hCmdBuffer);
  }

  void CopyBufferToImage(APIWindow* _pWindow, VkBuffer _hBuffer, VkImage _hImage, uint32_t _uWidth, uint32_t _uHeight)
  {
    VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);

    VkBufferImageCopy oCopyRegion{};
    oCopyRegion.bufferOffset = 0;
    oCopyRegion.bufferRowLength = 0;
    oCopyRegion.bufferImageHeight = 0;
    oCopyRegion.imageOffset = { 0, 0, 0 };
    oCopyRegion.imageExtent = { _uWidth, _uHeight, 1 };
    oCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    oCopyRegion.imageSubresource.baseArrayLayer = 0;
    oCopyRegion.imageSubresource.layerCount = 1;
    oCopyRegion.imageSubresource.mipLevel = 0;

    vkCmdCopyBufferToImage(hCmdBuffer, _hBuffer, _hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &oCopyRegion);

    EndTempCmdBuffer(_pWindow, hCmdBuffer);
  }

  void TransitionImageLayout(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, VkImageAspectFlags _uAspectFlags, VkImageLayout _eOldLayout, VkImageLayout _eNewLayout)
  {
    VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);

    VkImageMemoryBarrier oBarrier{};
    oBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    oBarrier.oldLayout = _eOldLayout;
    oBarrier.newLayout = _eNewLayout;
    oBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    oBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    oBarrier.image = _hImage;
    oBarrier.subresourceRange.aspectMask = _uAspectFlags;
    oBarrier.subresourceRange.baseMipLevel = 0;
    oBarrier.subresourceRange.levelCount = _uMipLevels;
    oBarrier.subresourceRange.baseArrayLayer = 0;
    oBarrier.subresourceRange.layerCount = 1;    

    VkPipelineStageFlags uSrcStage;
    VkPipelineStageFlags uDstStage;

    if (_eOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _eNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
      oBarrier.srcAccessMask = 0;
      oBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      uSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      uDstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (_eOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _eNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
      oBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      oBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      uSrcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      uDstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (_eOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _eNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
      oBarrier.srcAccessMask = 0;
      oBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      uSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      uDstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
      THROW_GENERIC_EXCEPTION("[API] Unsupported layout transition")
    }

    vkCmdPipelineBarrier(hCmdBuffer, uSrcStage, uDstStage, 0, 0, NULL, 0, NULL, 1, &oBarrier);

    EndTempCmdBuffer(_pWindow, hCmdBuffer);
  }

  void GenerateMipmaps(APIWindow* _pWindow, VkImage _hImage, int32_t _iWidth, int32_t _iHeight, uint32_t _uMipLevels)
  {
    VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);

    VkImageMemoryBarrier oBarrier{};
    oBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    oBarrier.image = _hImage;
    oBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    oBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    oBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    oBarrier.subresourceRange.baseArrayLayer = 0;
    oBarrier.subresourceRange.layerCount = 1;
    oBarrier.subresourceRange.levelCount = 1;

    int32_t iMipWidth = _iWidth;
    int32_t iMipHeight = _iHeight;

    for (uint32_t i = 1; i < _uMipLevels; i++)
    {
      oBarrier.subresourceRange.baseMipLevel = i - 1;
      oBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      oBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      oBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      oBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(hCmdBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &oBarrier);

      VkImageBlit oBlit{};
      oBlit.srcOffsets[0] = { 0, 0, 0 };
      oBlit.srcOffsets[1] = { iMipWidth, iMipHeight, 1 };
      oBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      oBlit.srcSubresource.mipLevel = i - 1;
      oBlit.srcSubresource.baseArrayLayer = 0;
      oBlit.srcSubresource.layerCount = 1;
      oBlit.dstOffsets[0] = { 0, 0, 0 };
      oBlit.dstOffsets[1] = { iMipWidth > 1 ? iMipWidth / 2 : 1, iMipHeight > 1 ? iMipHeight / 2 : 1, 1 };
      oBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      oBlit.dstSubresource.mipLevel = i;
      oBlit.dstSubresource.baseArrayLayer = 0;
      oBlit.dstSubresource.layerCount = 1;

      vkCmdBlitImage(hCmdBuffer,
        _hImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        _hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1u, &oBlit,
        VK_FILTER_LINEAR);

      oBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      oBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;;
      oBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      oBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(hCmdBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &oBarrier);

      if (iMipWidth > 1) iMipWidth /= 2;
      if (iMipHeight > 1) iMipHeight /= 2;

    }

    oBarrier.subresourceRange.baseMipLevel = _uMipLevels - 1;
    oBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    oBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;;
    oBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    oBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(hCmdBuffer,
      VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
      0, nullptr,
      0, nullptr,
      1, &oBarrier);

    EndTempCmdBuffer(_pWindow, hCmdBuffer);
  }

  VkFormat GetVKFormat(ImageFormat _eFormat)
  {
    switch (_eFormat)
    {
    case ImageFormat::R8G8B8:
      return VK_FORMAT_R8G8B8_SRGB;
    case ImageFormat::R8G8B8A8:
      return VK_FORMAT_R8G8B8A8_SRGB;
    default:
      break;
    }
    return VK_FORMAT_MAX_ENUM;
  }
  
  VkShaderStageFlagBits GetVkStageFlag(PipelineStage _eStage)
  {
    switch (_eStage)
    {
    case PipelineStage::VERTEX:
      return VK_SHADER_STAGE_VERTEX_BIT;
      break;
    case PipelineStage::PIXEL:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
      break;
    default:
      break;
    }
    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  }

  ///----------------------------------------------------------------------------------
  //                                    API functions
  ///----------------------------------------------------------------------------------

  // General

  void InitializeAPI()
  {
    CreateInstance();
    CreatePhysicalDevice();    
  }

  void ShutDownAPI()
  {    
    vkDestroyInstance(s_oGlobalData.m_hInstance, NULL);
  }

  // Window

  APIWindow* CreateAPIWindow(GLFWwindow* _pGlfwWindow)
  {
    APIWindow* pWindow = new APIWindow();

    CreateLogicalDevice(pWindow);
    RetrieveQueues(pWindow);
    CreateSurface(pWindow, _pGlfwWindow);
    CreateCommandBuffers(pWindow);
    CreateSwapchain(pWindow);
    CreateColorBuffer(pWindow);
    CreateDepthBuffer(pWindow);    
    CreateRenderPass(pWindow);
    CreateFramebuffers(pWindow);    
    CreateSyncObjects(pWindow);  
    CreateDescriptorPool(pWindow);
    CreateGlobalDescriptorLayout(pWindow);

    if (s_oGlobalData.m_pUsingWindow == NULL)
    {
      s_oGlobalData.m_pUsingWindow = pWindow;
    }

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

  void DestroyAPIWindow(APIWindow* _pWindow)
  {

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, 1, &_pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    DestroySwapchain(_pWindow);

    vkDestroySemaphore(_pWindow->m_hDevice, _pWindow->m_hImageAvailableSemaphore, NULL);
    vkDestroySemaphore(_pWindow->m_hDevice, _pWindow->m_hRenderFinishedSemaphore, NULL);

    vkDestroyFence(_pWindow->m_hDevice, _pWindow->m_hInFlightFence, NULL);
        

    vkDestroyDescriptorSetLayout(_pWindow->m_hDevice, _pWindow->m_hGlobalDescSetLayout, NULL);    

    vkDestroyCommandPool(_pWindow->m_hDevice, _pWindow->m_hRenderCmdPool, NULL);
    if (_pWindow->m_hRenderCmdPool != _pWindow->m_hPresentCmdPool)
    {
      vkDestroyCommandPool(_pWindow->m_hDevice, _pWindow->m_hPresentCmdPool, NULL);
    }

    vkDestroyRenderPass(_pWindow->m_hDevice, _pWindow->m_hRenderPass, NULL);     

    vkDestroyDescriptorPool(_pWindow->m_hDevice, _pWindow->m_hDescPool, NULL);    

    vkDestroySurfaceKHR(s_oGlobalData.m_hInstance, _pWindow->m_hSurface, NULL);    

    vkDestroyDevice(_pWindow->m_hDevice, NULL);   

    delete _pWindow;

  }

  // Camera

  APICamera* CreateAPICamera()
  {
    APICamera* pCamera = new APICamera();

    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

    pCamera->m_pOwnerWindow = pWindow;

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    std::vector<VkDescriptorSetLayout> lstLayouts;
    for (int i = 0; i < uNumImages; i++)
    {
      lstLayouts.push_back(pWindow->m_hGlobalDescSetLayout);
    }

    VkDescriptorSetAllocateInfo oDescSetAllocInfo{};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = pWindow->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = lstLayouts.data();

    pCamera->m_pDescSets = new VkDescriptorSet[uNumImages];

    VK_CHECK(vkAllocateDescriptorSets(pWindow->m_hDevice, &oDescSetAllocInfo, pCamera->m_pDescSets))

    return pCamera;
  }

  void BeginCameraSubStateSetup(APICamera* _pCamera)
  {
    s_oGlobalData.m_pUsingWindow = _pCamera->m_pOwnerWindow;
  }

  void EndCameraSubstateSetup(APICamera* _pCamera)
  {
    APIWindow* pWindow = _pCamera->m_pOwnerWindow;
    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;
    s_oGlobalData.m_oDescSetUpdater.Update(pWindow->m_hDevice, _pCamera->m_pDescSets, uNumImages, s_oGlobalData.m_oGlobalLayoutBuilder);
    s_oGlobalData.m_oDescSetUpdater.Clear();
  }

  void BindAPICamera(APICamera* _pCamera)
  {    
    APIRenderState* pRenderState = s_oGlobalData.m_pUsingRenderState;
    APIWindow* pWindow = _pCamera->m_pOwnerWindow;
    uint32_t uImageIdx = pWindow->m_uCurrSwapchainImageIdx;
    vkCmdBindDescriptorSets(pWindow->m_hRenderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pRenderState->m_hPipelineLayout, static_cast<uint32_t>(ResourceFrequency::GLOBAL), 1, &_pCamera->m_pDescSets[uImageIdx], 0, NULL);
  }

  void DestroyAPICamera(APICamera* _pCamera)
  {

    delete[] _pCamera->m_pDescSets;

    delete _pCamera;
  }

  // Mesh

  APIMesh* CreateAPIMesh(void* _pVertexData, size_t _uVertexDataSize, void* _pIndexData, size_t _uIndexDataSize)
  {
    APIMesh* pMesh = new APIMesh();

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

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, 1, &pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    DestroyBuffer(_pMesh->m_pOwnerWindow, _pMesh->m_hVertexBuffer, _pMesh->m_hVertexBufferMemory);
    DestroyBuffer(_pMesh->m_pOwnerWindow, _pMesh->m_hIndexBuffer, _pMesh->m_hIndexBufferMemory);

    delete _pMesh;
  }

  // Constant buffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
  {
    APIConstantBuffer* pCBuffer = new APIConstantBuffer();

    pCBuffer->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

    APIWindow* pWindow = pCBuffer->m_pOwnerWindow;    
    
    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    // Create buffers

    pCBuffer->m_pUniformBuffers = new VkBuffer[uNumImages];
    pCBuffer->m_pUniformBuffersMemory = new VkDeviceMemory[uNumImages];
    pCBuffer->m_pUniformBuffersMapped = new void*[uNumImages];

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

    uint32_t uCurrImageIdx = pWindow->m_uCurrSwapchainImageIdx;

    memcpy(_pCBuffer->m_pUniformBuffersMapped[uCurrImageIdx], _pData, _uSize);
  }

  void BindAPIConstantBuffer(APIConstantBuffer* /*_pCbuffer*/)
  {
    
  }

  void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer)
  {

    APIWindow* pWindow = _pCBuffer->m_pOwnerWindow;    

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, 1, &pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VkDevice& hDevice = pWindow->m_hDevice;

    for (uint32_t i = 0; i < uNumImages; i++)
    {
      vkUnmapMemory(hDevice, _pCBuffer->m_pUniformBuffersMemory[i]);
      DestroyBuffer(pWindow, _pCBuffer->m_pUniformBuffers[i], _pCBuffer->m_pUniformBuffersMemory[i]);
    }

    delete[] _pCBuffer->m_pUniformBuffers;
    delete[] _pCBuffer->m_pUniformBuffersMemory;
    delete[] _pCBuffer->m_pUniformBuffersMapped;

    delete _pCBuffer;
  }

  // Texture

  APITexture* CreateAPITexture(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, uint32_t _uMipLevels)
  {

    APITexture* pTexture = new APITexture();

    pTexture->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;

    APIWindow* pWindow = pTexture->m_pOwnerWindow;

    VkBuffer hStagingBuffer;
    VkDeviceMemory hStagingBufferMemory;       

    size_t uSize = _uWidth* _uHeight* GetImageFormatSize(_eFormat);

    VkFormat eVkFormat = GetVKFormat(_eFormat);

    uint32_t uMaxMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_uWidth, _uWidth)))) + 1;
    _uMipLevels = _uMipLevels == 0u? uMaxMipLevels : std::min(_uMipLevels, uMaxMipLevels);

    CreateBuffer(pWindow, uSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, hStagingBuffer, hStagingBufferMemory);

    SetBufferData(pWindow, _pData, uSize, hStagingBufferMemory);    

    CreateImage(pWindow, _uWidth, _uHeight, eVkFormat, _uMipLevels, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pTexture->m_hImage, pTexture->m_hMemory);

    TransitionImageLayout(pWindow, pTexture->m_hImage, eVkFormat, _uMipLevels, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    CopyBufferToImage(pWindow, hStagingBuffer, pTexture->m_hImage, _uWidth, _uHeight);    

    GenerateMipmaps(pWindow, pTexture->m_hImage, _uWidth, _uHeight, _uMipLevels);

    //TransitionImageLayout(pWindow, pTexture->m_hImage, eVkFormat, _uMipLevels, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    DestroyBuffer(pWindow, hStagingBuffer, hStagingBufferMemory);

    CreateImageView(pWindow, pTexture->m_hImage, eVkFormat, _uMipLevels, VK_IMAGE_ASPECT_COLOR_BIT, pTexture->m_hImageView);

    CreateTextureSampler(pWindow, pTexture, _uMipLevels);

    return pTexture;
  }

  void BindAPITexture(APITexture* /*_pTexture*/)
  {
    
  }

  void DestroyAPITexture(APITexture* _pTexture)
  {
    APIWindow* pWindow = _pTexture->m_pOwnerWindow;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, 1, &pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    vkDestroySampler(pWindow->m_hDevice, _pTexture->m_hSampler, NULL);
    vkDestroyImageView(pWindow->m_hDevice, _pTexture->m_hImageView, NULL);
    vkDestroyImage(pWindow->m_hDevice, _pTexture->m_hImage, NULL);
    vkFreeMemory(pWindow->m_hDevice, _pTexture->m_hMemory, NULL);

    delete _pTexture;
  }

  // Render state

  APIRenderState* CreateAPIRenderState(const RenderStateInfo& _oInfo)
  {
    APIRenderState* pRenderState = new APIRenderState();
    pRenderState->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;  
    uint32_t uNumImages = pRenderState->m_pOwnerWindow->m_uSwapchainImageCount;

    file::File oVSFile("Assets/Shaders/Vulkan/Vertex/VertexShader.spv");
    file::File oPSFile("Assets/Shaders/Vulkan/Pixel/PixelShader.spv");    

    ReflectSetLayouts(oVSFile, pRenderState->m_oMaterialLayoutBuilder, pRenderState->m_oMatInstanceLayoutBuilder);
    ReflectSetLayouts(oPSFile, pRenderState->m_oMaterialLayoutBuilder, pRenderState->m_oMatInstanceLayoutBuilder);

    VkDevice hDevice = pRenderState->m_pOwnerWindow->m_hDevice;
    
    pRenderState->m_hDescSetLayout = pRenderState->m_oMaterialLayoutBuilder.Build(hDevice);
    pRenderState->m_hSubDescSetLayout = pRenderState->m_oMatInstanceLayoutBuilder.Build(hDevice);

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

    pRenderState->m_pDescSets = new VkDescriptorSet[uNumImages];    

    VK_CHECK(vkAllocateDescriptorSets(hDevice, &oDescSetAllocInfo, pRenderState->m_pDescSets))

    CreatePipeline(oVSFile, oPSFile, _oInfo, pRenderState);

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

    s_oGlobalData.m_oDescSetUpdater.Update(pWindow->m_hDevice, pRenderState->m_pDescSets, uNumImages, pRenderState->m_oMaterialLayoutBuilder);
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
    vkCmdBindPipeline(pWindow->m_hRenderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hGraphicsPipeline);

    VkViewport oViewport{};
    oViewport.x = 0.0f;
    oViewport.y = 0.0f;
    oViewport.width = (float)pWindow->m_oExtent.width;
    oViewport.height = (float)pWindow->m_oExtent.height;
    oViewport.minDepth = 0.0f;
    oViewport.maxDepth = 1.0f;

    vkCmdSetViewport(pWindow->m_hRenderCmdBuffer, 0, 1, &oViewport);

    VkRect2D oScissor{};
    oScissor.offset = { 0, 0 };
    oScissor.extent = pWindow->m_oExtent;

    vkCmdSetScissor(pWindow->m_hRenderCmdBuffer, 0, 1, &oScissor);

    uint32_t uImageIdx = pWindow->m_uCurrSwapchainImageIdx;
    vkCmdBindDescriptorSets(pWindow->m_hRenderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hPipelineLayout, static_cast<uint32_t>(ResourceFrequency::MATERIAL), 1, &_pAPIRenderState->m_pDescSets[uImageIdx], 0, NULL);    

    s_oGlobalData.m_pUsingRenderState = _pAPIRenderState;
  }

  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    APIWindow* pWindow = _pAPIRenderState->m_pOwnerWindow;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, 1, &pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    delete[] _pAPIRenderState->m_pDescSets;    

    vkDestroyDescriptorSetLayout(pWindow->m_hDevice, _pAPIRenderState->m_hDescSetLayout, NULL);
    vkDestroyDescriptorSetLayout(pWindow->m_hDevice, _pAPIRenderState->m_hSubDescSetLayout, NULL);

    vkDestroyPipelineLayout(pWindow->m_hDevice, _pAPIRenderState->m_hPipelineLayout, NULL);

    vkDestroyPipeline(pWindow->m_hDevice, _pAPIRenderState->m_hGraphicsPipeline, NULL);

    delete _pAPIRenderState;
  }  

  APIRenderSubState* CreateAPIRenderSubState()
  {    
    APIRenderSubState* pSubState = new APIRenderSubState();
    pSubState->m_pRenderState = s_oGlobalData.m_pUsingRenderState;
    return pSubState;
  }

  void BeginSubStateSetup(APIRenderSubState* _pSubState)
  {
    if (s_oGlobalData.m_pUsingSubState != nullptr)
    {
      THROW_GENERIC_EXCEPTION("[API] EndSubStateSetup was not called before a new BeginSubStateSetup call")
    }

    s_oGlobalData.m_pUsingSubState = _pSubState;
    s_oGlobalData.m_pUsingWindow = _pSubState->m_pRenderState->m_pOwnerWindow;

    uint32_t uNumImages = s_oGlobalData.m_pUsingWindow->m_uSwapchainImageCount;

    std::vector<VkDescriptorSetLayout> lstLayouts;
    for (int i = 0; i < uNumImages; i++)
    {
      lstLayouts.push_back(_pSubState->m_pRenderState->m_hSubDescSetLayout);
    }

    VkDescriptorSetAllocateInfo oDescSetAllocInfo{};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = s_oGlobalData.m_pUsingWindow->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = lstLayouts.data();

    _pSubState->m_pDescSets = new VkDescriptorSet[uNumImages];

    VK_CHECK(vkAllocateDescriptorSets(s_oGlobalData.m_pUsingWindow->m_hDevice, &oDescSetAllocInfo, _pSubState->m_pDescSets))
  }

  void SubStateSetupConstantBuffer(APIConstantBuffer* _pCBuffer, size_t _uSize, const ResourceBindInfo& _oBindInfo)
  {    

    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    for (int i = 0; i < uNumImages; i++)
    {
      VkDescriptorBufferInfo oBufferInfo{};
      oBufferInfo.buffer = _pCBuffer->m_pUniformBuffers[i];
      oBufferInfo.offset = 0;
      oBufferInfo.range = _uSize;

      s_oGlobalData.m_oDescSetUpdater.AddBufferInfo(std::move(oBufferInfo), _oBindInfo.m_iBinding, i);
    }
  }

  void SubStateSetupTexture(APITexture* _pTexture, const ResourceBindInfo& _oBindInfo)
  {    

    APIWindow* pWindow = s_oGlobalData.m_pUsingWindow;

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    for (int i = 0; i < uNumImages; i++)
    {
      VkDescriptorImageInfo oImageInfo{};
      oImageInfo.sampler = _pTexture->m_hSampler;
      oImageInfo.imageView = _pTexture->m_hImageView;
      oImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      s_oGlobalData.m_oDescSetUpdater.AddImageInfo(std::move(oImageInfo), _oBindInfo.m_iBinding, i);      
    }
  }

  void EndSubStateSetup()
  { 

    APIRenderSubState* pSubState = s_oGlobalData.m_pUsingSubState;

    APIRenderState* pRenderState = pSubState->m_pRenderState;

    APIWindow* pWindow = pRenderState->m_pOwnerWindow;

    uint32_t uNumImages = pRenderState->m_pOwnerWindow->m_uSwapchainImageCount;        

    s_oGlobalData.m_oDescSetUpdater.Update(pWindow->m_hDevice, pSubState->m_pDescSets, uNumImages, pRenderState->m_oMatInstanceLayoutBuilder);

    s_oGlobalData.m_oDescSetUpdater.Clear();

    s_oGlobalData.m_pUsingSubState = nullptr;    
  }

  void BindAPIRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    APIRenderState* pRenderState = _pAPIRenderSubState->m_pRenderState;
    APIWindow* pWindow = pRenderState->m_pOwnerWindow;
    uint32_t uImageIdx = pWindow->m_uCurrSwapchainImageIdx;
    vkCmdBindDescriptorSets(pWindow->m_hRenderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pRenderState->m_hPipelineLayout, static_cast<uint32_t>(ResourceFrequency::MATERIAL_INSTANCE), 1, &_pAPIRenderSubState->m_pDescSets[uImageIdx], 0, NULL);

    s_oGlobalData.m_pUsingSubState = _pAPIRenderSubState;
  }

  void DestroyRenderSubState(APIRenderSubState* _pAPIRenderSubState)
  {
    /*APIWindow* pWindow = _pAPIRenderSubState->m_pRenderState->m_pOwnerWindow;

    VK_CHECK(vkWaitForFences(pWindow->m_hDevice, 1, &pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    uint32_t uNumImages = pWindow->m_uSwapchainImageCount;
    vkFreeDescriptorSets(pWindow->m_hDevice, pWindow->m_hDescPool, uNumImages, _pAPIRenderSubState->m_pDescSets);*/

    delete _pAPIRenderSubState;
  }

  // Drawing

  int BeginDraw(APIWindow* _pWindow)
  {

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, 1, &_pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    VkResult hResult = VK_SUCCESS; 
    
    if (!_pWindow->m_bResized)
    {
      hResult = vkAcquireNextImageKHR(_pWindow->m_hDevice, _pWindow->m_hSwapchain, UINT64_MAX, _pWindow->m_hImageAvailableSemaphore, VK_NULL_HANDLE, &_pWindow->m_uCurrSwapchainImageIdx);
    }

    if (hResult == VK_ERROR_OUT_OF_DATE_KHR || hResult == VK_SUBOPTIMAL_KHR || _pWindow->m_bResized)
    {
      _pWindow->m_bResized = false;      
      RecreateSwapchain(_pWindow);      
      return 1;
    }
    else if (hResult != VK_SUCCESS)
    {
      VK_CHECK(hResult)
    }

    VK_CHECK(vkResetFences(_pWindow->m_hDevice, 1, &_pWindow->m_hInFlightFence))

    VK_CHECK(vkResetCommandBuffer(_pWindow->m_hRenderCmdBuffer, 0))

    VkCommandBufferBeginInfo oCommandBufferBeginInfo {};
    oCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    oCommandBufferBeginInfo.flags = 0u;
    oCommandBufferBeginInfo.pInheritanceInfo = NULL;

    VK_CHECK(vkBeginCommandBuffer(_pWindow->m_hRenderCmdBuffer, &oCommandBufferBeginInfo))

    VkClearValue aClearColors[3];
    aClearColors[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    aClearColors[1].depthStencil = { 1.0f, 0 };
    aClearColors[2].color = { {0.0f, 0.0f, 0.0f, 1.0f} };

    VkRenderPassBeginInfo oRenderPassBeginInfo {};
    oRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    oRenderPassBeginInfo.renderPass = _pWindow->m_hRenderPass;
    oRenderPassBeginInfo.framebuffer = _pWindow->m_pFramebuffers[_pWindow->m_uCurrSwapchainImageIdx];
    oRenderPassBeginInfo.renderArea.extent = _pWindow->m_oExtent;
    oRenderPassBeginInfo.renderArea.offset = { 0,0 };    
    oRenderPassBeginInfo.clearValueCount = 3u;
    oRenderPassBeginInfo.pClearValues = aClearColors;    

    vkCmdBeginRenderPass(_pWindow->m_hRenderCmdBuffer, &oRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);    

    s_oGlobalData.m_pUsingWindow = _pWindow;

    return 0;
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount, void* _pConstantData, uint32_t _uConstantSize)
  {

    VkCommandBuffer& hCmdBuffer = _pMesh->m_pOwnerWindow->m_hRenderCmdBuffer;

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

    vkCmdEndRenderPass(_pWindow->m_hRenderCmdBuffer);

    VK_CHECK(vkEndCommandBuffer(_pWindow->m_hRenderCmdBuffer))

    VkSemaphore aWaitSemaphores[] = { _pWindow->m_hImageAvailableSemaphore };
    VkPipelineStageFlags aWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore aSignalSemaphores[] = { _pWindow->m_hRenderFinishedSemaphore };

    VkSubmitInfo oSubmitInfo{};
    oSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    oSubmitInfo.waitSemaphoreCount = 1;
    oSubmitInfo.pWaitSemaphores = aWaitSemaphores;
    oSubmitInfo.pWaitDstStageMask = aWaitStages;
    oSubmitInfo.commandBufferCount = 1u;
    oSubmitInfo.pCommandBuffers = &_pWindow->m_hRenderCmdBuffer;
    oSubmitInfo.signalSemaphoreCount = 1u;
    oSubmitInfo.pSignalSemaphores = aSignalSemaphores;

    VK_CHECK(vkQueueSubmit(_pWindow->m_hRenderQueue, 1, &oSubmitInfo, _pWindow->m_hInFlightFence))

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
  }

}
}