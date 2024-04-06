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
#include "Graphics/API/Vulkan/APIMesh.h"
#include "Graphics/API/Vulkan/APIConstantBuffer.h"
#include "Graphics/API/Vulkan/APIRenderState.h"
#include "File/File.h"

namespace api
{
namespace vk
{

  VulkanData s_oGlobalData;

  constexpr uint32_t QUEUE_COUNT = 1u;

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
  }

  void CreatePipeline(APIRenderState* _pRenderState)
  {

    APIWindow* pWindow = _pRenderState->m_pOwnerWindow;

    // Create shader modules
    
    VkPipelineShaderStageCreateInfo aStages[2];

    // Vertex stage    
    
    file::File oVSFile("Assets/Shaders/Vertex/VertexShader.spv");
    VkShaderModuleCreateInfo oVSCreateInfo{};
    oVSCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    oVSCreateInfo.codeSize = oVSFile.GetSize();
    oVSCreateInfo.pCode = reinterpret_cast<const uint32_t*>(oVSFile.GetData());

    VkShaderModule hVSShaderModule;
    VK_CHECK(vkCreateShaderModule(pWindow->m_hDevice, &oVSCreateInfo, NULL, &hVSShaderModule))

    aStages[0] = {};
    aStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    aStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    aStages[0].module = hVSShaderModule;
    aStages[0].pName = "main";
          
    // Pixel stage
    
    file::File oPSFile("Assets/Shaders/Pixel/PixelShader.spv");
    VkShaderModuleCreateInfo oPSCreateInfo{};
    oPSCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    oPSCreateInfo.codeSize = oPSFile.GetSize();
    oPSCreateInfo.pCode = reinterpret_cast<const uint32_t*>(oPSFile.GetData());

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

    // Viewport and scissors fixed state
    
    VkViewport oViewport{};
    oViewport.x = 0.0f;
    oViewport.y = 0.0f;
    oViewport.width = (float)pWindow->m_oExtent.width;
    oViewport.height = (float)pWindow->m_oExtent.height;
    oViewport.minDepth = 0.0f;
    oViewport.maxDepth = 1.0f;    

    VkRect2D oScissor{};
    oScissor.offset = { 0, 0 };
    oScissor.extent = pWindow->m_oExtent;    

    VkPipelineViewportStateCreateInfo oViewportState{};
    oViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    oViewportState.viewportCount = 1;
    oViewportState.pViewports = &oViewport;
    oViewportState.scissorCount = 1;
    oViewportState.pScissors = &oScissor;

    // Dynamic state for viewport and scissors

    /*VkDynamicState aDynamicStates [] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo oDynamicState{};
    oDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    oDynamicState.dynamicStateCount = static_cast<uint32_t>(sizeof(aDynamicStates) / sizeof(VkDynamicState));
    oDynamicState.pDynamicStates = aDynamicStates;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;*/

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
    oMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    oMultisampling.minSampleShading = 1.0f; // Optional
    oMultisampling.pSampleMask = NULL; // Optional
    oMultisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    oMultisampling.alphaToOneEnable = VK_FALSE; // Optional

    // Depth stencil (disabled)

    VkPipelineDepthStencilStateCreateInfo oDepthStencil {};
    oDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    oDepthStencil.depthTestEnable = VK_FALSE;
    oDepthStencil.depthWriteEnable = VK_FALSE;
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

    // Pipeline layout (uniforms)     

    VkDescriptorSetLayoutBinding oUboLayoutBinding{};
    oUboLayoutBinding.binding = 0;
    oUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oUboLayoutBinding.descriptorCount = 1;
    oUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    oUboLayoutBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo oDescSetLayoutInfo {};
    oDescSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    oDescSetLayoutInfo.bindingCount = 1u;
    oDescSetLayoutInfo.pBindings = &oUboLayoutBinding;

    VK_CHECK(vkCreateDescriptorSetLayout(pWindow->m_hDevice, &oDescSetLayoutInfo, NULL, &_pRenderState->m_hDescSetLayout))

    VkPipelineLayoutCreateInfo oPipelineLayoutCreateInfo {};
    oPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    oPipelineLayoutCreateInfo.setLayoutCount = 1u;
    oPipelineLayoutCreateInfo.pSetLayouts = &_pRenderState->m_hDescSetLayout;
    oPipelineLayoutCreateInfo.pushConstantRangeCount = 0u;
    oPipelineLayoutCreateInfo.pPushConstantRanges = NULL;

    VK_CHECK(vkCreatePipelineLayout(pWindow->m_hDevice, &oPipelineLayoutCreateInfo, NULL, &_pRenderState->m_hPipelineLayout))

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
    oPipelineInfo.layout = _pRenderState->m_hPipelineLayout;
    oPipelineInfo.renderPass = pWindow->m_hRenderPass;
    oPipelineInfo.subpass = 0u;
    oPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    oPipelineInfo.basePipelineIndex = -1; // Optional

    VK_CHECK(vkCreateGraphicsPipelines(pWindow->m_hDevice, VK_NULL_HANDLE, 1u, &oPipelineInfo, NULL, &_pRenderState->m_hGraphicsPipeline))

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

    VkDeviceCreateInfo oDeviceCreateInfo = {};
    oDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    oDeviceCreateInfo.queueCreateInfoCount = uQueueInfoCount;
    oDeviceCreateInfo.pQueueCreateInfos = &aDeviceQueueCreateInfos[0];
    oDeviceCreateInfo.ppEnabledLayerNames = NULL;
    oDeviceCreateInfo.enabledLayerCount = 0u;
    oDeviceCreateInfo.enabledExtensionCount = uDeviceExtensionCount;
    oDeviceCreateInfo.ppEnabledExtensionNames = aDeviceExtensions;    

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
    oColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    oColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    oColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    oColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    oColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    oColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    oColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference oColorAttachmentRef{};
    oColorAttachmentRef.attachment = 0u;
    oColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription oSubpassDesc{};
    oSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    oSubpassDesc.colorAttachmentCount = 1u;
    oSubpassDesc.pColorAttachments = &oColorAttachmentRef;

    VkSubpassDependency oSubpassDependency{};
    oSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    oSubpassDependency.dstSubpass = 0u;
    oSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    oSubpassDependency.srcAccessMask = 0;
    oSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    oSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo oRenderPassCreateInfo{};
    oRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    oRenderPassCreateInfo.attachmentCount = 1u;
    oRenderPassCreateInfo.pAttachments = &oColorAttachmentDesc;
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
      VkImageView pAttachments[] = { _pWindow->m_pSwapChainImageViews[i] };

      VkFramebufferCreateInfo oFramebufferInfo = {};
      oFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      oFramebufferInfo.attachmentCount = 1u;
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

  void DestroySwapchain(APIWindow* _pWindow)
  {
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

  void OnWindowResize(APIWindow* _pWindow)
  {
    _pWindow->m_bResized = true;
  }

  void RecreateSwapchain(APIWindow* _pWindow)
  {
    DestroySwapchain(_pWindow);
    CreateSwapchain(_pWindow);
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

  void DestroyBuffer(APIWindow* _pWindow, VkBuffer _hBuffer, VkDeviceMemory _hDeviceMemory_)
  {
    vkDestroyBuffer(_pWindow->m_hDevice, _hBuffer, NULL);
    vkFreeMemory(_pWindow->m_hDevice, _hDeviceMemory_, NULL);
  }

  void SetBufferData(APIWindow* _pWindow, void* _pData, size_t _uSize, VkDeviceMemory& hDeviceMemory_)
  {
    VkDevice hDevice = _pWindow->m_hDevice;
    void* pDeviceData;
    VK_CHECK(vkMapMemory(hDevice, hDeviceMemory_, 0, _uSize, 0, &pDeviceData))
    memcpy(pDeviceData, _pData, _uSize);
    vkUnmapMemory(hDevice, hDeviceMemory_);
  }

  void CopyBuffer(APIWindow* _pWindow, VkBuffer _hSrcBuffer, VkBuffer _hDstBuffer, VkDeviceSize _uSize)
  {
    VkCommandBufferAllocateInfo oAllocInfo{};
    oAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    oAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    oAllocInfo.commandPool = _pWindow->m_hRenderCmdPool;
    oAllocInfo.commandBufferCount = 1;

    VkCommandBuffer hCommandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(_pWindow->m_hDevice, &oAllocInfo, &hCommandBuffer))

    VkCommandBufferBeginInfo oBeginInfo{};
    oBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    oBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(hCommandBuffer, &oBeginInfo))

    VkBufferCopy oCopyRegion{};
    oCopyRegion.srcOffset = 0; // Optional
    oCopyRegion.dstOffset = 0; // Optional
    oCopyRegion.size = _uSize;
    vkCmdCopyBuffer(hCommandBuffer, _hSrcBuffer, _hDstBuffer, 1, &oCopyRegion);

    VK_CHECK(vkEndCommandBuffer(hCommandBuffer))

    VkSubmitInfo oSubmitInfo{};
    oSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    oSubmitInfo.commandBufferCount = 1;
    oSubmitInfo.pCommandBuffers = &hCommandBuffer;

    VK_CHECK(vkQueueSubmit(_pWindow->m_hRenderQueue, 1, &oSubmitInfo, VK_NULL_HANDLE))
    VK_CHECK(vkQueueWaitIdle(_pWindow->m_hRenderQueue))

    vkFreeCommandBuffers(_pWindow->m_hDevice, _pWindow->m_hRenderCmdPool, 1u, &hCommandBuffer);
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
    CreateSwapchain(pWindow);
    CreateRenderPass(pWindow);
    CreateFramebuffers(pWindow);
    CreateCommandBuffers(pWindow);    
    CreateSyncObjects(pWindow);  

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

  void DestroyAPIWindow(APIWindow* _pWindow)
  {

    DestroySwapchain(_pWindow);

    vkDestroySemaphore(_pWindow->m_hDevice, _pWindow->m_hImageAvailableSemaphore, NULL);
    vkDestroySemaphore(_pWindow->m_hDevice, _pWindow->m_hRenderFinishedSemaphore, NULL);

    vkDestroyFence(_pWindow->m_hDevice, _pWindow->m_hInFlightFence, NULL);

    vkDestroyCommandPool(_pWindow->m_hDevice, _pWindow->m_hRenderCmdPool, NULL);
    vkDestroyCommandPool(_pWindow->m_hDevice, _pWindow->m_hPresentCmdPool, NULL);    

    vkDestroyRenderPass(_pWindow->m_hDevice, _pWindow->m_hRenderPass, NULL);     

    vkDestroyDevice(_pWindow->m_hDevice, NULL);   

    delete _pWindow;

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
    DestroyBuffer(_pMesh->m_pOwnerWindow, _pMesh->m_hVertexBuffer, _pMesh->m_hVertexBufferMemory);

    delete _pMesh;
  }

  // Constant buffer

  APIConstantBuffer* CreateAPIConstantBuffer(size_t _uSize)
  {
    APIConstantBuffer* pCBuffer = new APIConstantBuffer();

    pCBuffer->m_pOwnerRenderState = s_oGlobalData.m_pUsingRenderState;

    APIWindow* pWindow = pCBuffer->m_pOwnerRenderState->m_pOwnerWindow;    
    
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

    // Create descriptor pool

    VkDescriptorPoolSize oDescPoolSize{};
    oDescPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oDescPoolSize.descriptorCount = uNumImages;

    VkDescriptorPoolCreateInfo oDescPoolInfo {};
    oDescPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    oDescPoolInfo.flags = 0;
    oDescPoolInfo.poolSizeCount = 1;
    oDescPoolInfo.pPoolSizes = &oDescPoolSize;
    oDescPoolInfo.maxSets = uNumImages;

    VK_CHECK(vkCreateDescriptorPool(pWindow->m_hDevice, &oDescPoolInfo, NULL, &pCBuffer->m_hDescPool))

    // Allocate descriptor sets

    VkDescriptorSetLayout* pLayouts = new VkDescriptorSetLayout[uNumImages];
    for (int i = 0; i < uNumImages; i++)
    {
      pLayouts[i] = pCBuffer->m_pOwnerRenderState->m_hDescSetLayout;
    }

    pCBuffer->m_pDescSets = new VkDescriptorSet[uNumImages];

    VkDescriptorSetAllocateInfo oDescSetAllocInfo {};
    oDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    oDescSetAllocInfo.descriptorPool = pCBuffer->m_hDescPool;
    oDescSetAllocInfo.descriptorSetCount = uNumImages;
    oDescSetAllocInfo.pSetLayouts = pLayouts;

    VK_CHECK(vkAllocateDescriptorSets(pWindow->m_hDevice, &oDescSetAllocInfo, pCBuffer->m_pDescSets ))

    // Configure descriptor sets

    for (int i = 0; i < uNumImages; i++)
    {
      VkDescriptorBufferInfo oBufferInfo{};
      oBufferInfo.buffer = pCBuffer->m_pUniformBuffers[i];
      oBufferInfo.offset = 0;
      oBufferInfo.range = _uSize;

      VkWriteDescriptorSet oWriteDescSet{};
      oWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      oWriteDescSet.dstSet = pCBuffer->m_pDescSets[i];
      oWriteDescSet.dstBinding = 0;
      oWriteDescSet.dstArrayElement = 0;
      oWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      oWriteDescSet.descriptorCount = 1;
      oWriteDescSet.pBufferInfo = &oBufferInfo;
      oWriteDescSet.pImageInfo = NULL;
      oWriteDescSet.pTexelBufferView = NULL;

      vkUpdateDescriptorSets(pWindow->m_hDevice, 1, &oWriteDescSet, 0, nullptr);
    }

    delete[] pLayouts;

    return pCBuffer;
  }

  void UpdateAPIConstanBuffer(APIConstantBuffer* _pCBuffer, const void* _pData, size_t _uSize)
  {
    APIWindow* pWindow = _pCBuffer->m_pOwnerRenderState->m_pOwnerWindow;

    uint32_t uCurrImageIdx = pWindow->m_uCurrSwapchainImageIdx;

    memcpy(_pCBuffer->m_pUniformBuffersMapped[uCurrImageIdx], _pData, _uSize);
  }

  void BindAPIConstantBuffer(APIConstantBuffer* _pCbuffer)
  {
    APIRenderState* pRenderState = _pCbuffer->m_pOwnerRenderState;
    APIWindow* pWindow = pRenderState->m_pOwnerWindow;
    uint32_t uImageIdx = pWindow->m_uCurrSwapchainImageIdx;
    vkCmdBindDescriptorSets(pWindow->m_hRenderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pRenderState->m_hPipelineLayout, 0, 1, &_pCbuffer->m_pDescSets[uImageIdx], 0, NULL);
  }

  void DestroyAPIConstanBuffer(APIConstantBuffer* _pCBuffer)
  {

    APIWindow* pWindow = _pCBuffer->m_pOwnerRenderState->m_pOwnerWindow;

    const uint32_t uNumImages = pWindow->m_uSwapchainImageCount;

    VkDevice& hDevice = pWindow->m_hDevice;

    for (uint32_t i = 0; i < uNumImages; i++)
    {
      vkUnmapMemory(hDevice, _pCBuffer->m_pUniformBuffersMemory[i]);
      DestroyBuffer(pWindow, _pCBuffer->m_pUniformBuffers[i], _pCBuffer->m_pUniformBuffersMemory[i]);
    }

    delete[] _pCBuffer->m_pDescSets;

    vkDestroyDescriptorPool(hDevice, _pCBuffer->m_hDescPool, nullptr);    

    delete[] _pCBuffer->m_pUniformBuffers;
    delete[] _pCBuffer->m_pUniformBuffersMemory;
    delete[] _pCBuffer->m_pUniformBuffersMapped;

    delete _pCBuffer;
  }

  // Render state

  APIRenderState* CreateAPIRenderState()
  {
    APIRenderState* pRenderState = new APIRenderState();
    pRenderState->m_pOwnerWindow = s_oGlobalData.m_pUsingWindow;
    CreatePipeline(pRenderState);

    return pRenderState;
  }

  void BindAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    APIWindow* pWindow = _pAPIRenderState->m_pOwnerWindow;
    vkCmdBindPipeline(pWindow->m_hRenderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pAPIRenderState->m_hGraphicsPipeline);
  }

  void DestroyAPIRenderState(APIRenderState* _pAPIRenderState)
  {
    APIWindow* pWindow = _pAPIRenderState->m_pOwnerWindow;

    vkDestroyDescriptorSetLayout(pWindow->m_hDevice, _pAPIRenderState->m_hDescSetLayout, NULL);

    vkDestroyPipelineLayout(pWindow->m_hDevice, _pAPIRenderState->m_hPipelineLayout, NULL);

    delete _pAPIRenderState;
  }

  void SetUsingRenderState(APIRenderState* _pRenderState)
  {
    s_oGlobalData.m_pUsingRenderState = _pRenderState;
  }

  // Drawing

  int BeginDraw(APIWindow* _pWindow)
  {

    VK_CHECK(vkWaitForFences(_pWindow->m_hDevice, 1, &_pWindow->m_hInFlightFence, VK_TRUE, UINT64_MAX))

    VkResult hResult = vkAcquireNextImageKHR(_pWindow->m_hDevice, _pWindow->m_hSwapchain, UINT64_MAX, _pWindow->m_hImageAvailableSemaphore, VK_NULL_HANDLE, &_pWindow->m_uCurrSwapchainImageIdx);

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

    VkRenderPassBeginInfo oRenderPassBeginInfo {};
    oRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    oRenderPassBeginInfo.renderPass = _pWindow->m_hRenderPass;
    oRenderPassBeginInfo.framebuffer = _pWindow->m_pFramebuffers[_pWindow->m_uCurrSwapchainImageIdx];
    oRenderPassBeginInfo.renderArea.extent = _pWindow->m_oExtent;
    oRenderPassBeginInfo.renderArea.offset = { 0,0 };
    VkClearValue oClearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    oRenderPassBeginInfo.pClearValues = &oClearColor;
    oRenderPassBeginInfo.clearValueCount = 1u;

    vkCmdBeginRenderPass(_pWindow->m_hRenderCmdBuffer, &oRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);    

    return 0;
  }

  void DrawMesh(APIMesh* _pMesh, uint32_t _uIndexCount)
  {

    VkCommandBuffer& hCmdBuffer = _pMesh->m_pOwnerWindow->m_hRenderCmdBuffer;

    VkBuffer vertexBuffers[] = { _pMesh->m_hVertexBuffer };
    VkDeviceSize offsets[] = { 0u };
    vkCmdBindVertexBuffers(hCmdBuffer, 0u, 1u, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(hCmdBuffer, _pMesh->m_hIndexBuffer, 0u, VK_INDEX_TYPE_UINT16);

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