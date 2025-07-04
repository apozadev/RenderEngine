#include "Graphics/API/Vulkan/APIInternal.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "Graphics/ResourceFrequency.h"
#include "Graphics/TextureUsage.h"
#include "Graphics/API/Vulkan/VulkanAPI.h"
#include "Graphics/API/Vulkan/VulkanMacros.h"
#include "Graphics/API/Vulkan/VulkanShaderReflection.h"
#include "Graphics/API/Vulkan/VulkanPools.h"

namespace api
{
namespace vk
{

VulkanData s_oGlobalData;


VkFormat GetVKFormat(ImageFormat _eFormat)
{
  switch (_eFormat)
  {
  case ImageFormat::R32:
    return VK_FORMAT_D32_SFLOAT;
  case ImageFormat::R8G8B8:
    return VK_FORMAT_R8G8B8_UNORM;
  case ImageFormat::R16G16B16:
    return VK_FORMAT_R16G16B16_SFLOAT;
  case ImageFormat::R32G32B32:
    return VK_FORMAT_R32G32B32_SFLOAT;
  case ImageFormat::R8G8B8_SRGB:
    return VK_FORMAT_R8G8B8_SRGB;
  case ImageFormat::R32G32B32A32:
    return VK_FORMAT_R32G32B32A32_SFLOAT;
  case ImageFormat::R8G8B8A8:
    return VK_FORMAT_R8G8B8A8_UNORM;
  case ImageFormat::R8G8B8A8_SRGB:
    return VK_FORMAT_R8G8B8A8_SRGB;
  default:
    break;
  }
  return VK_FORMAT_MAX_ENUM;
}

VkShaderStageFlags GetVkShaderFlags(PipelineStageFlags _eStageFlags)
{

  VkShaderStageFlags uFlag = 0u;

  if ((_eStageFlags & STAGE_VERTEX) != 0)
  {
    uFlag |= VK_SHADER_STAGE_VERTEX_BIT;
  }

  if ((_eStageFlags & STAGE_PIXEL) != 0)
  {
    uFlag |= VK_SHADER_STAGE_FRAGMENT_BIT;
  }

  if ((_eStageFlags & STAGE_GEOM) != 0)
  {
    uFlag |= VK_SHADER_STAGE_GEOMETRY_BIT;
  }

  return uFlag;
}

VkBlendOp GetVkBlendOp(BlendOp _eBlendOp)
{
  switch (_eBlendOp)
  {
  case BLEND_OP_ADD:
    return VK_BLEND_OP_ADD;
  case BLEND_OP_SUBTRACT:
    return VK_BLEND_OP_SUBTRACT;
  case BLEND_OP_REV_SUBTRACT:
    return VK_BLEND_OP_REVERSE_SUBTRACT;
  case BLEND_OP_MIN:
    return VK_BLEND_OP_MIN;
  case BLEND_OP_MAX:
    return VK_BLEND_OP_MAX;
  default:
    return VK_BLEND_OP_ADD;
  }
}

VkBlendFactor GetVkBlendFactor(BlendFactor _eBlendFactor)
{
  switch (_eBlendFactor)
  {
  case BLEND_ZERO:
    return VK_BLEND_FACTOR_ZERO;
  case BLEND_ONE:
    return VK_BLEND_FACTOR_ONE;
  case BLEND_SRC_COLOR:
    return VK_BLEND_FACTOR_SRC_COLOR;
  case BLEND_INV_SRC_COLOR:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
  case BLEND_DEST_COLOR:
    return VK_BLEND_FACTOR_DST_COLOR;
  case BLEND_INV_DEST_COLOR:
    return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
  case BLEND_SRC_ALPHA:
    return VK_BLEND_FACTOR_SRC_ALPHA;
  case BLEND_INV_SRC_ALPHA:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  case BLEND_DEST_ALPHA:
    return VK_BLEND_FACTOR_DST_ALPHA;
  case BLEND_INV_DEST_ALPHA:
    return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
  case BLEND_SRC_ALPHA_SAT:
    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
  case BLEND_SRC1_COLOR:
    return VK_BLEND_FACTOR_SRC1_COLOR;
  case BLEND_INV_SRC1_COLOR:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
  case BLEND_SRC1_ALPHA:
    return VK_BLEND_FACTOR_SRC1_ALPHA;
  case BLEND_INV_SRC1_ALPHA:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
  default:
    return VK_BLEND_FACTOR_ZERO;
  }
}

VkImageUsageFlags GetVkTextureUsage(uint32_t _uUsage)
{
  VkImageUsageFlags uRes = 0u;

  if ((_uUsage & static_cast<int>(TextureUsage::SHADER_RESOURCE)) != 0)
  {
    uRes |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }
  if ((_uUsage & static_cast<int>(TextureUsage::COLOR_TARGET)) != 0)
  {
    uRes |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }
  if ((_uUsage & static_cast<int>(TextureUsage::DEPTH_TARGET)) != 0)
  {
    uRes |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }
  if ((_uUsage & static_cast<int>(TextureUsage::TRANSFER_SRC)) != 0)
  {
    uRes |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }
  if ((_uUsage & static_cast<int>(TextureUsage::TRANSFER_DST)) != 0)
  {
    uRes |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  return uRes;
}

VkSamplerAddressMode GetVkAddressMode(TextureAddressMode _eAddressMode)
{
  switch (_eAddressMode)
  {
  case TextureAddressMode::REPEAT:
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  case TextureAddressMode::MIRRORED_REPEAT:
    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  case TextureAddressMode::CLAMP:
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  case TextureAddressMode::BORDER:
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  default:
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  }
}

VkSamplerMipmapMode GetSamplerMipmapMode(TextureFilterMode _eMipmapMode)
{
  switch (_eMipmapMode)
  {
  case TextureFilterMode::LINEAR:
    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
  case TextureFilterMode::POINT:
    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
  default:
    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
  }
}

VkFilter GetVkFilter(TextureFilterMode _eFliterMode)
{
  switch (_eFliterMode)
  {
  case TextureFilterMode::LINEAR:
    return VK_FILTER_LINEAR;
  case TextureFilterMode::POINT:
    return VK_FILTER_NEAREST;
  default:
    return VK_FILTER_NEAREST;
  }
}

VkCullModeFlagBits GetVkCullMode(CullMode _eCullMode)
{
  switch (_eCullMode)
  {
  case CullMode::BACK:
    return VK_CULL_MODE_BACK_BIT;
  case CullMode::FRONT:
    return VK_CULL_MODE_FRONT_BIT;
  case CullMode::NONE:
    return VK_CULL_MODE_NONE;
  default:
    return VK_CULL_MODE_BACK_BIT;
  }
}

VkCompareOp GetVkCompareOp(DepthCompareOp _eCompareOp)
{
  switch (_eCompareOp)
  {
  case DepthCompareOp::LESS:
    return VK_COMPARE_OP_LESS;
  case DepthCompareOp::EQUAL:
    return VK_COMPARE_OP_EQUAL;
  case DepthCompareOp::LESS_OR_EQUAL:
    return VK_COMPARE_OP_LESS_OR_EQUAL;  
  case DepthCompareOp::GREATER:
    return VK_COMPARE_OP_GREATER;
  case DepthCompareOp::NOT_EQUAL:
    return VK_COMPARE_OP_NOT_EQUAL;
  case DepthCompareOp::GREATER_OR_EQUAL:
    return VK_COMPARE_OP_GREATER_OR_EQUAL;
  case DepthCompareOp::ALWAYS:
    return VK_COMPARE_OP_ALWAYS;
  case DepthCompareOp::NEVER:
    return VK_COMPARE_OP_NEVER;
  default:
    return VK_COMPARE_OP_LESS;
  }
}

void CreateInstance()
{

  const char* pDebugLayers[] = { "VK_LAYER_KHRONOS_validation"};


#ifdef _DEBUG
  uint32_t uLayerCount = 1u;
#else
  uint32_t uLayerCount = 0u;
#endif

  std::vector<const char*> lstExtensions;
  {
    uint32_t uGlfwExtensionCount = 0u;
    const char** pGlfwExtensions;
    pGlfwExtensions = glfwGetRequiredInstanceExtensions(&uGlfwExtensionCount);
    for (int i = 0; i < uGlfwExtensionCount; i++)
    {
      lstExtensions.push_back(pGlfwExtensions[i]);
    }
  }

  VkInstanceCreateInfo oInstanceCreateInfo = {};
  oInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  oInstanceCreateInfo.pNext = NULL;
  oInstanceCreateInfo.flags = 0u;
  oInstanceCreateInfo.enabledLayerCount = uLayerCount;
  oInstanceCreateInfo.ppEnabledLayerNames = pDebugLayers;
  oInstanceCreateInfo.enabledExtensionCount = lstExtensions.size();
  oInstanceCreateInfo.ppEnabledExtensionNames = lstExtensions.data();

  VkApplicationInfo oApplicationInfo = {};
  oApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  oApplicationInfo.apiVersion = VK_API_VERSION_1_3;
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

uint32_t FindBinding(const APIWindow* _pWindow, const ResourceBindInfo& _oBindInfo, VkDescriptorType _eType)
{
  uint32_t uBinding = 0xFFFFFFFFu;

  switch (_oBindInfo.m_eLevel)
  {
  case ResourceFrequency::GLOBAL:
    uBinding = _pWindow->m_oGlobalLayoutBuilder.FindBinding(_oBindInfo.m_sName, GetVkShaderFlags(_oBindInfo.m_uStageFlags), _eType);
    break;
  case ResourceFrequency::RENDER_STEP:
    uBinding = _pWindow->m_oStepLayoutBuilder.FindBinding(_oBindInfo.m_sName, GetVkShaderFlags(_oBindInfo.m_uStageFlags), _eType);
    break;
  case ResourceFrequency::MATERIAL_INSTANCE:
    uBinding = _pWindow->m_oMatInstanceLayoutBuilder.FindBinding(_oBindInfo.m_sName, GetVkShaderFlags(_oBindInfo.m_uStageFlags), _eType);
    break;
  default:
    break;
  }

  if (uBinding == 0xFFFFFFFFu)
  {    
    THROW_GENERIC_EXCEPTION((_oBindInfo.m_sName + ": A descriptor with such name was not found").c_str());
  }

  return uBinding;
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

    s_oGlobalData.m_uAPIMayorVersion = VK_API_VERSION_MAJOR(oDeviceProperties.apiVersion);
    s_oGlobalData.m_uAPIMinorVersion = VK_API_VERSION_MINOR(oDeviceProperties.apiVersion);

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

void CreatePipeline(const APIWindow* _pWindow,
  const file::InFile& _oVSFile,
  const file::InFile& _oPSFile,
  const file::InFile* _pGSFile,
  const RenderStateInfo& _oInfo,
  VkRenderPass _hRenderPass,
  VkSampleCountFlagBits _uMsaaSamples,
  APIRenderState* _pRenderState_)
{

  // Create shader modules

  uint32_t uStageCount = 2u;
  VkPipelineShaderStageCreateInfo aStages[3];

  // Vertex stage    

  VkShaderModuleCreateInfo oVSCreateInfo{};
  oVSCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  oVSCreateInfo.codeSize = _oVSFile.GetSize();
  oVSCreateInfo.pCode = reinterpret_cast<const uint32_t*>(_oVSFile.GetData());

  VkShaderModule hVSShaderModule;
  VK_CHECK(vkCreateShaderModule(_pWindow->m_hDevice, &oVSCreateInfo, NULL, &hVSShaderModule))

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
  VK_CHECK(vkCreateShaderModule(_pWindow->m_hDevice, &oPSCreateInfo, NULL, &hPSShaderModule))

  aStages[1] = {};
  aStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  aStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  aStages[1].module = hPSShaderModule;
  aStages[1].pName = "main";

  // Geometry stage

  VkShaderModule hGSShaderModule = VK_NULL_HANDLE;

  if (_pGSFile)
  {
    VkShaderModuleCreateInfo oGSCreateInfo{};
    oGSCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    oGSCreateInfo.codeSize = _pGSFile->GetSize();
    oGSCreateInfo.pCode = reinterpret_cast<const uint32_t*>(_pGSFile->GetData());
    
    VK_CHECK(vkCreateShaderModule(_pWindow->m_hDevice, &oGSCreateInfo, NULL, &hGSShaderModule))

    aStages[2] = {};
    aStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    aStages[2].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
    aStages[2].module = hGSShaderModule;
    aStages[2].pName = "main";

    uStageCount++;    
  }

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

  //VkPipelineViewportDepthClipControlCreateInfoEXT oDepthClipControl = {};
  //oDepthClipControl.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_DEPTH_CLIP_CONTROL_CREATE_INFO_EXT;
  //oDepthClipControl.negativeOneToOne = VK_TRUE;  // Enables DirectX-style [-1, 1] depth range

  VkPipelineViewportStateCreateInfo oViewportState = {};
  oViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  oViewportState.viewportCount = 1;
  oViewportState.pViewports = nullptr; // Set to nullptr for dynamic state
  oViewportState.scissorCount = 1;
  oViewportState.pScissors = nullptr;  // Set to nullptr for dynamic state
  //oViewportState.pNext = &oDepthClipControl;

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
  oRasterizer.cullMode = GetVkCullMode(_oInfo.m_eCullMode);
  oRasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  oRasterizer.depthBiasEnable = VK_FALSE;
  oRasterizer.depthBiasConstantFactor = 0.0f; // Optional
  oRasterizer.depthBiasClamp = 0.0f; // Optional
  oRasterizer.depthBiasSlopeFactor = 0.0f; // Optional

  // Multisampling

  VkSampleLocationEXT customSampleLocations[8] = {
      {0.5625f * 0.9375f, 0.3125f * 0.9375f},  // (1, -3) 
      {0.4375f * 0.9375f, 0.6875f * 0.9375f},  // (-1, 3) 
      {0.8125f * 0.9375f, 0.5625f * 0.9375f},  // (5, 1) 
      {0.3125f * 0.9375f, 0.1875f * 0.9375f},  // (-3, -5) 
      {0.1875f * 0.9375f, 0.8125f * 0.9375f},  // (-5, 5) 
      {0.0625f * 0.9375f, 0.4375f * 0.9375f},  // (-7, -1) 
      {0.6875f * 0.9375f, 0.9375f * 0.9375f},  // (3, 7) 
      {0.9375f * 0.9375f, 0.0625f * 0.9375f}   // (7, -7) 
  };

  // Describe the sample locations
  //VkSampleLocationsInfoEXT sampleLocInfo = {};
  //sampleLocInfo.sType = VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT;
  //sampleLocInfo.sampleLocationsPerPixel = VK_SAMPLE_COUNT_8_BIT; // 8x MSAA
  //sampleLocInfo.sampleLocationGridSize = { 1, 1 }; // Single sample location grid
  //sampleLocInfo.sampleLocationsCount = 8;
  //sampleLocInfo.pSampleLocations = customSampleLocations;

  //// Enable sample locations in pipeline creation
  //VkPipelineSampleLocationsStateCreateInfoEXT sampleLocState = {};
  //sampleLocState.sType = VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT;
  //sampleLocState.sampleLocationsEnable = VK_TRUE;
  //sampleLocState.sampleLocationsInfo = sampleLocInfo;  

  VkSampleMask uSampleMask = 0xFFFFFFFF;

  VkPipelineMultisampleStateCreateInfo oMultisampling{};
  oMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  oMultisampling.sampleShadingEnable = VK_FALSE;
  oMultisampling.rasterizationSamples = _uMsaaSamples;
  oMultisampling.minSampleShading = 1.0f; // Optional
  oMultisampling.pSampleMask = &uSampleMask; // Optional
  oMultisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  oMultisampling.alphaToOneEnable = VK_FALSE; // Optional
  /*if (_uMsaaSamples > VK_SAMPLE_COUNT_1_BIT)
  {
    oMultisampling.pNext = &sampleLocState;
  }*/

  // Depth stencil 

  VkPipelineDepthStencilStateCreateInfo oDepthStencil{};
  oDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  oDepthStencil.depthTestEnable = _oInfo.m_bDepthRead;
  oDepthStencil.depthWriteEnable = _oInfo.m_bDepthWrite;
  oDepthStencil.depthCompareOp = GetVkCompareOp(_oInfo.m_eDepthCompareOp);
  oDepthStencil.depthBoundsTestEnable = VK_FALSE;
  oDepthStencil.stencilTestEnable = VK_FALSE;
  oDepthStencil.minDepthBounds = 0.f;
  oDepthStencil.maxDepthBounds = 1.f;

  // Blending

  VkPipelineColorBlendAttachmentState oBlendAttachment{};
  oBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  oBlendAttachment.blendEnable = _oInfo.m_bBlendEnabled ? VK_TRUE : VK_FALSE;
  oBlendAttachment.srcColorBlendFactor = GetVkBlendFactor(_oInfo.m_eSrcBlendFactor);
  oBlendAttachment.dstColorBlendFactor = GetVkBlendFactor(_oInfo.m_eDstBlendFactor);
  oBlendAttachment.colorBlendOp = GetVkBlendOp(_oInfo.m_eBlendOp);
  oBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  oBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  oBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo oColorBlending{};
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

  VkDescriptorSetLayout aSetLayouts[4] =
  {
    _pWindow->m_hGlobalDescSetLayout
    , _pWindow->m_hStepDescSetLayout
    , _pRenderState_->m_hDescSetLayout
    , _pWindow->m_hMatInstanceDescSetLayout
  };

  // Push constants

  VkPushConstantRange push_constant;
  push_constant.offset = 0;
  push_constant.size = _oInfo.m_uMeshConstantSize;
  push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkPipelineLayoutCreateInfo oPipelineLayoutCreateInfo{};
  oPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  oPipelineLayoutCreateInfo.setLayoutCount = 4u;
  oPipelineLayoutCreateInfo.pSetLayouts = aSetLayouts;
  oPipelineLayoutCreateInfo.pushConstantRangeCount = 1u;
  oPipelineLayoutCreateInfo.pPushConstantRanges = &push_constant;

  VK_CHECK(vkCreatePipelineLayout(_pWindow->m_hDevice, &oPipelineLayoutCreateInfo, NULL, &_pRenderState_->m_hPipelineLayout))

  // Create Pipeline object

  VkGraphicsPipelineCreateInfo oPipelineInfo{};
  oPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  oPipelineInfo.stageCount = uStageCount;
  oPipelineInfo.pStages = aStages;
  oPipelineInfo.pVertexInputState = &oVertexInputInfo;
  oPipelineInfo.pInputAssemblyState = &oInputAssemblyInfo;
  oPipelineInfo.pTessellationState = &oTesselationInfo;
  oPipelineInfo.pViewportState = &oViewportState;
  oPipelineInfo.pRasterizationState = &oRasterizer;
  oPipelineInfo.pMultisampleState = &oMultisampling;
  oPipelineInfo.pDepthStencilState = &oDepthStencil; // Optional
  oPipelineInfo.pColorBlendState = &oColorBlending;
  oPipelineInfo.layout = _pRenderState_->m_hPipelineLayout;
  oPipelineInfo.renderPass = _hRenderPass;
  oPipelineInfo.subpass = 0u;
  oPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  oPipelineInfo.basePipelineIndex = -1; // Optional
  oPipelineInfo.pDynamicState = &oDynamicState;

  VK_CHECK(vkCreateGraphicsPipelines(_pWindow->m_hDevice, VK_NULL_HANDLE, 1u, &oPipelineInfo, NULL, &_pRenderState_->m_hGraphicsPipeline))

  vkDestroyShaderModule(_pWindow->m_hDevice, hVSShaderModule, NULL);  
  vkDestroyShaderModule(_pWindow->m_hDevice, hPSShaderModule, NULL);
  if (hGSShaderModule != VK_NULL_HANDLE)
  {
    vkDestroyShaderModule(_pWindow->m_hDevice, hGSShaderModule, NULL);
  }
}

void CreateLogicalDevice(APIWindow* _pWindow)
{

  const char* aDeviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  //VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME
  VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME,
  VK_KHR_MULTIVIEW_EXTENSION_NAME
  };

  uint32_t uDeviceExtensionCount = 3u;  

  VkDeviceQueueCreateInfo aDeviceQueueCreateInfos[2];

  unsigned int uQueueInfoCount;

  if (s_oGlobalData.m_uRenderQueueFamilyIdx == s_oGlobalData.m_uPresentQueueFamilyIdx)
  {
    uQueueInfoCount = 1;

    static const float queuePriorities[] = { 1.0f, 1.0f };

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

  // Check extensions are available

  uint32_t uSupportedExtensionCount = 0;
  vkEnumerateDeviceExtensionProperties(s_oGlobalData.m_hPhysicalDevice, nullptr, &uSupportedExtensionCount, nullptr);

  std::vector<VkExtensionProperties> lstSupportedExtensions(uSupportedExtensionCount);
  vkEnumerateDeviceExtensionProperties(s_oGlobalData.m_hPhysicalDevice, nullptr, &uSupportedExtensionCount, lstSupportedExtensions.data());

  for (int i = 0; i < uDeviceExtensionCount; i++)
  {
    bool found = false;
    for (const auto& ext : lstSupportedExtensions)
    {
      if (std::strcmp(ext.extensionName, aDeviceExtensions[i]) == 0)
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      THROW_GENERIC_EXCEPTION("Some extension not supported by this GPU!");
    }
  }

  VkPhysicalDeviceDepthClipControlFeaturesEXT oDepthClipControlFeatures = {};
  oDepthClipControlFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT;

  VkPhysicalDeviceSampleLocationsPropertiesEXT oSampleLocationsFeatures = {};
  oSampleLocationsFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT;

  VkPhysicalDeviceMultiviewFeaturesKHR oMultiviewFeatures{};
  oMultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
  oMultiviewFeatures.multiview = VK_TRUE;

  VkPhysicalDeviceFeatures2 oDeviceFeatures2 = {};
  oDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  oDeviceFeatures2.pNext = &oSampleLocationsFeatures;

  VkPhysicalDeviceFeatures oEnabledFeatures = {};
  oEnabledFeatures.geometryShader = VK_TRUE;
  oEnabledFeatures.samplerAnisotropy = VK_TRUE;  

  vkGetPhysicalDeviceFeatures2(s_oGlobalData.m_hPhysicalDevice, &oDeviceFeatures2);

  VkDeviceCreateInfo oDeviceCreateInfo = {};
  oDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  oDeviceCreateInfo.queueCreateInfoCount = uQueueInfoCount;
  oDeviceCreateInfo.pQueueCreateInfos = &aDeviceQueueCreateInfos[0];
  oDeviceCreateInfo.ppEnabledLayerNames = NULL;
  oDeviceCreateInfo.enabledLayerCount = 0u;
  oDeviceCreateInfo.enabledExtensionCount = uDeviceExtensionCount;
  oDeviceCreateInfo.ppEnabledExtensionNames = aDeviceExtensions;
  oDeviceCreateInfo.pEnabledFeatures = &oEnabledFeatures;
  oDeviceCreateInfo.pNext = &oMultiviewFeatures;

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
  CreateImage(_pWindow
    , _pWindow->m_oExtent.width
    , _pWindow->m_oExtent.height
    , _pWindow->m_eSwapchainFormat
    , 1u
    , 1u
    , _pWindow->m_eMsaaSamples
    , VK_IMAGE_TILING_OPTIMAL
    , VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
    , 0u    
    , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    , _pWindow->m_hColorImage
    , _pWindow->m_hColorImageMemory);

  CreateImageView(_pWindow, _pWindow->m_hColorImage, _pWindow->m_eSwapchainFormat, 0u, 1u, 0u, 1u, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, _pWindow->m_hColorImageView);
}

void CreateDepthBuffer(APIWindow* _pWindow)
{

  VkFormat eFormat = VK_FORMAT_D32_SFLOAT;
  VkImageAspectFlags uAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

  CreateImage(_pWindow,
    _pWindow->m_oExtent.width,
    _pWindow->m_oExtent.height,
    eFormat,
    1u,
    1u,
    _pWindow->m_eMsaaSamples,
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT,//0u,    
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    _pWindow->m_hDepthImage,
    _pWindow->m_hDepthImageMemory);

  CreateImageView(_pWindow, _pWindow->m_hDepthImage, eFormat, 0u, 1u, 0u, 1u, VK_IMAGE_VIEW_TYPE_2D, uAspectFlags, _pWindow->m_hDepthImageView);

  VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);
  TransitionImageLayout(hCmdBuffer, _pWindow->m_hDepthImage, eFormat, 0u, 1u, 0u, 1u, uAspectFlags, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  EndTempCmdBuffer(_pWindow, hCmdBuffer);
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

void CreateRenderPass(const APIWindow* _pWindow, uint32_t _uNumColorTextures, VkFormat _eColorFormat, bool _bHasDepthStencil, VkFormat _eDepthStencilFormat, uint32_t _uMsaaSampleCount, VkRenderPass& hRenderPass_, bool _bOffscreen, bool _bMultiview)
{

  bool bHasMsaa = _uMsaaSampleCount > 1u;

  VkAttachmentDescription oColorAttachmentDesc{};
  oColorAttachmentDesc.format = _eColorFormat;
  oColorAttachmentDesc.samples = static_cast<VkSampleCountFlagBits>(_uMsaaSampleCount);
  oColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  oColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  oColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  oColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  oColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  if (bHasMsaa)
  {
    oColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  else
  {
    oColorAttachmentDesc.finalLayout = _bOffscreen ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  }

  VkAttachmentDescription oColorResolveAttachmentDesc{};
  oColorResolveAttachmentDesc.format = oColorAttachmentDesc.format;
  oColorResolveAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  oColorResolveAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  oColorResolveAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  oColorResolveAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  oColorResolveAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  oColorResolveAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  oColorResolveAttachmentDesc.finalLayout = _bOffscreen ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 

  VkAttachmentDescription oDepthAttachmentDesc{};
  oDepthAttachmentDesc.format = _eDepthStencilFormat;
  oDepthAttachmentDesc.samples = static_cast<VkSampleCountFlagBits>(_uMsaaSampleCount);
  oDepthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  oDepthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  oDepthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  oDepthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  oDepthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;  

  if (bHasMsaa)
  {
    oDepthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }
  else
  {
    oDepthAttachmentDesc.finalLayout = _bOffscreen ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }


  VkAttachmentReference* pColorAttachmentRefs = s_oVkAttachmentReferencePool.PullElements(_uNumColorTextures);
  VkAttachmentReference* pColorResolveAttachmentRefs = s_oVkAttachmentReferencePool.PullElements(_uNumColorTextures);

  for (uint32_t i = 0; i < _uNumColorTextures; i++)
  {
    pColorAttachmentRefs[i] = {};
    pColorAttachmentRefs[i].attachment = i;
    pColorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    pColorResolveAttachmentRefs[i] = {};
    pColorResolveAttachmentRefs[i].attachment = i + _uNumColorTextures;
    pColorResolveAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  VkAttachmentReference oDepthAttachmentRef{};
  oDepthAttachmentRef.attachment = _uNumColorTextures + (bHasMsaa ? _uNumColorTextures : 0u);
  oDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription oSubpassDesc{};
  oSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  oSubpassDesc.colorAttachmentCount = _uNumColorTextures;
  oSubpassDesc.pColorAttachments = pColorAttachmentRefs;
  oSubpassDesc.pDepthStencilAttachment = _bHasDepthStencil ? &oDepthAttachmentRef : nullptr;
  oSubpassDesc.pResolveAttachments = bHasMsaa ? pColorResolveAttachmentRefs : nullptr;

  VkSubpassDependency oSubpassDependency{};
  oSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  oSubpassDependency.dstSubpass = 0u;
  oSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  oSubpassDependency.srcAccessMask = 0;
  oSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  oSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  const uint32_t uNumAttachments = _uNumColorTextures + (bHasMsaa ? _uNumColorTextures : 0u) + (_bHasDepthStencil ? 1u : 0u);

  VkAttachmentDescription* pAttachments = s_oVkAttachmentDescriptionPool.PullElements(uNumAttachments);

  uint32_t uAttachmentIdx = 0u;

  for (int i = 0; i < _uNumColorTextures; i++)
  {
    pAttachments[uAttachmentIdx++] = oColorAttachmentDesc;
  }

  if (bHasMsaa)
  {
    for (int i = 0; i < _uNumColorTextures; i++)
    {
      pAttachments[uAttachmentIdx++] = oColorResolveAttachmentDesc;
    }
  }

  if (_bHasDepthStencil)
  {
    pAttachments[uAttachmentIdx++] = oDepthAttachmentDesc;
  }

  /*
    Bit mask that specifies which view rendering is broadcast to
    0011 = Broadcast to first and second view (layer)
  */
  const uint32_t viewMask = 0b00111111;

  /*
    Bit mask that specifies correlation between views
    An implementation may use this for optimizations (concurrent render)
  */
  const uint32_t correlationMask = 0b00111111;

  VkRenderPassMultiviewCreateInfo oRenderPassMultiViewCreateInfo{};
  oRenderPassMultiViewCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
  oRenderPassMultiViewCreateInfo.subpassCount = 1;
  oRenderPassMultiViewCreateInfo.pViewMasks = &viewMask;
  oRenderPassMultiViewCreateInfo.correlationMaskCount = 1;
  oRenderPassMultiViewCreateInfo.pCorrelationMasks = &correlationMask;  

  VkRenderPassCreateInfo oRenderPassCreateInfo{};
  oRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  oRenderPassCreateInfo.attachmentCount = uNumAttachments;
  oRenderPassCreateInfo.pAttachments = pAttachments;
  oRenderPassCreateInfo.subpassCount = 1u;
  oRenderPassCreateInfo.pSubpasses = &oSubpassDesc;
  oRenderPassCreateInfo.dependencyCount = 1u;
  oRenderPassCreateInfo.pDependencies = &oSubpassDependency;

  if (_bMultiview)
  {
    oRenderPassCreateInfo.pNext = &oRenderPassMultiViewCreateInfo;
  }

  VK_CHECK(vkCreateRenderPass(_pWindow->m_hDevice, &oRenderPassCreateInfo, NULL, &hRenderPass_))

  if(pAttachments) s_oVkAttachmentDescriptionPool.ReturnElements(pAttachments);
  if(pColorAttachmentRefs) s_oVkAttachmentReferencePool.ReturnElements(pColorAttachmentRefs);
  if(pColorResolveAttachmentRefs) s_oVkAttachmentReferencePool.ReturnElements(pColorResolveAttachmentRefs);

}

void CreateCommandBuffers(APIWindow* _pWindow)
{
  VkCommandPoolCreateInfo oCmdPoolCreateInfo = {};
  oCmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  oCmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  oCmdPoolCreateInfo.queueFamilyIndex = s_oGlobalData.m_uRenderQueueFamilyIdx;
  oCmdPoolCreateInfo.pNext = NULL;
  VK_CHECK(vkCreateCommandPool(_pWindow->m_hDevice, &oCmdPoolCreateInfo, NULL, &_pWindow->m_hCmdPool))

  _pWindow->m_pCmdBuffers = new VkCommandBuffer[_pWindow->m_uSwapchainImageCount];

  // Create render command buffer
  {
    VkCommandBufferAllocateInfo oCmdBufferAllocInfo = {};
    oCmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    oCmdBufferAllocInfo.commandPool = _pWindow->m_hCmdPool;
    oCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    oCmdBufferAllocInfo.commandBufferCount = _pWindow->m_uSwapchainImageCount;
    VK_CHECK(vkAllocateCommandBuffers(_pWindow->m_hDevice, &oCmdBufferAllocInfo, _pWindow->m_pCmdBuffers))
  }
}

void CreateSyncObjects(APIWindow* _pWindow)
{

  const uint32_t uNumImages = _pWindow->m_uSwapchainImageCount;

  _pWindow->m_pImageAvailableSemaphores = new VkSemaphore[uNumImages];
  _pWindow->m_pRenderFinishedSemaphores = new VkSemaphore[uNumImages];
  _pWindow->m_pInFlightFences = new VkFence[uNumImages];

  VkSemaphoreCreateInfo oSemaphoreInfo{};
  oSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo oFenceInfo{};
  oFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  oFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < uNumImages; i++)
  {
    VK_CHECK(vkCreateSemaphore(_pWindow->m_hDevice, &oSemaphoreInfo, NULL, &_pWindow->m_pImageAvailableSemaphores[i]))
    VK_CHECK(vkCreateSemaphore(_pWindow->m_hDevice, &oSemaphoreInfo, NULL, &_pWindow->m_pRenderFinishedSemaphores[i]))
    VK_CHECK(vkCreateFence(_pWindow->m_hDevice, &oFenceInfo, NULL, &_pWindow->m_pInFlightFences[i]))
  }
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
  oPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  oPoolInfo.maxSets = 999u;
  oPoolInfo.poolSizeCount = 2u;
  oPoolInfo.pPoolSizes = aPoolSizes;

  VK_CHECK(vkCreateDescriptorPool(_pWindow->m_hDevice, &oPoolInfo, NULL, &_pWindow->m_hDescPool))
}

void CreateGlobalDescriptorLayout(APIWindow* _pWindow)
{
  {
    {
      VkDescriptorSetLayoutBinding oBinding{};
      oBinding.binding = 0u;
      oBinding.descriptorCount = 1u;
      oBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      oBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      oBinding.pImmutableSamplers = NULL;

      _pWindow->m_oGlobalLayoutBuilder.AddLayoutBinding("GlobalBuffer", std::move(oBinding));
    }

    char aNames[4][11] = { "ShadowMap0", "ShadowMap1", "ShadowMap2", "ShadowMap3" };

    for (uint32_t i = 0u; i < 4u; i++)
    {
      VkDescriptorSetLayoutBinding oBinding{};
      oBinding.binding = i + 1u;
      oBinding.descriptorCount = 1u;
      oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      oBinding.pImmutableSamplers = NULL;

      _pWindow->m_oGlobalLayoutBuilder.AddLayoutBinding(aNames[i], std::move(oBinding));
    }

    {
      VkDescriptorSetLayoutBinding oBinding{};
      oBinding.binding = 5u;
      oBinding.descriptorCount = 1u;
      oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      oBinding.pImmutableSamplers = NULL;

      _pWindow->m_oGlobalLayoutBuilder.AddLayoutBinding("SpecEnvMap", std::move(oBinding));
    }

    {
      VkDescriptorSetLayoutBinding oBinding{};
      oBinding.binding = 6u;
      oBinding.descriptorCount = 1u;
      oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      oBinding.pImmutableSamplers = NULL;

      _pWindow->m_oGlobalLayoutBuilder.AddLayoutBinding("DiffEnvMap", std::move(oBinding));
    }

    {
      {
        VkDescriptorSetLayoutBinding oBinding{};
        oBinding.binding = 7u;
        oBinding.descriptorCount = 1u;
        oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        oBinding.pImmutableSamplers = NULL;

        _pWindow->m_oGlobalLayoutBuilder.AddLayoutBinding("BrdfLutTex", std::move(oBinding));
      }
    }

    _pWindow->m_hGlobalDescSetLayout = _pWindow->m_oGlobalLayoutBuilder.Build(_pWindow->m_hDevice);
  }

  {

    char aNames[4][7] = { "Input0", "Input1", "Input2", "Input3" };

    for (uint32_t i = 0u; i < 4u; i++)
    {
      VkDescriptorSetLayoutBinding oBinding{};
      oBinding.binding = i;// static_cast<uint32_t>(ResourceFrequency::RENDER_STEP);
      oBinding.descriptorCount = 1u;
      oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      oBinding.pImmutableSamplers = NULL;

      _pWindow->m_oStepLayoutBuilder.AddLayoutBinding(aNames[i], std::move(oBinding));
    }

    VkDescriptorSetLayoutBinding oBinding{};
    oBinding.binding = 4u;
    oBinding.descriptorCount = 1u;
    oBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    oBinding.pImmutableSamplers = NULL;

    _pWindow->m_oStepLayoutBuilder.AddLayoutBinding("LightBuffer", std::move(oBinding));

    _pWindow->m_hStepDescSetLayout = _pWindow->m_oStepLayoutBuilder.Build(_pWindow->m_hDevice);
  }

  {

    char aNames[4][9] = { "Texture0", "Texture1", "Texture2", "Texture3" };

    for (uint32_t i = 0u; i < 4u; i++)
    {
      VkDescriptorSetLayoutBinding oBinding{};
      oBinding.binding = i;// static_cast<uint32_t>(ResourceFrequency::RENDER_STEP);
      oBinding.descriptorCount = 1u;
      oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      oBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      oBinding.pImmutableSamplers = NULL;

      _pWindow->m_oMatInstanceLayoutBuilder.AddLayoutBinding(aNames[i], std::move(oBinding));
    }

    _pWindow->m_hMatInstanceDescSetLayout = _pWindow->m_oMatInstanceLayoutBuilder.Build(_pWindow->m_hDevice);
  }
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

  bool bMsaa = _pWindow->m_eMsaaSamples > 1u;

  for (int i = 0; i < _pWindow->m_uSwapchainImageCount; i++)
  {
    uint32_t uAttachmentCount = 0u;

    VkImageView aImageViews[3];
    aImageViews[uAttachmentCount++] = bMsaa ? _pWindow->m_hColorImageView : _pWindow->m_pSwapChainImageViews[i];
    if (bMsaa)
    {
      aImageViews[uAttachmentCount++] = _pWindow->m_pSwapChainImageViews[i];
    }
    aImageViews[uAttachmentCount++] = _pWindow->m_hDepthImageView;


    VkFramebufferCreateInfo oFramebufferInfo = {};
    oFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    oFramebufferInfo.attachmentCount = uAttachmentCount;
    oFramebufferInfo.pAttachments = &aImageViews[0];
    oFramebufferInfo.renderPass = _pWindow->m_hRenderPass;
    oFramebufferInfo.width = _pWindow->m_oExtent.width;
    oFramebufferInfo.height = _pWindow->m_oExtent.height;
    oFramebufferInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(_pWindow->m_hDevice, &oFramebufferInfo, NULL, &_pWindow->m_pFramebuffers[i]))
  }
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

void CreateBuffer(const APIWindow* _pWindow
  , size_t _uSize
  , VkBufferUsageFlags _uUsage
  , VkMemoryPropertyFlags _uProperties
  , VkBuffer& hBuffer_
  , VkDeviceMemory& hDeviceMemory_)
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

void CreateImage(const APIWindow* _pWindow
  , uint32_t _uWidth
  , uint32_t _uHeight
  , VkFormat _eFormat
  , uint32_t _uMipLevels
  , uint32_t _uLayers
  , VkSampleCountFlagBits _eMsaaSampleCount
  , VkImageTiling _eTiling
  , VkImageUsageFlags _uUsage
  , VkImageCreateFlags _uCreateFlags  
  , VkMemoryPropertyFlags _uProperties
  , VkImage& hImage_
  , VkDeviceMemory& hMemory_)
{
  VkImageCreateInfo oImageCreateInfo{};

  oImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  oImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  oImageCreateInfo.extent.width = _uWidth;
  oImageCreateInfo.extent.height = _uHeight;
  oImageCreateInfo.extent.depth = 1u;
  oImageCreateInfo.mipLevels = _uMipLevels;
  oImageCreateInfo.arrayLayers = _uLayers;
  oImageCreateInfo.format = _eFormat;
  oImageCreateInfo.tiling = _eTiling;
  oImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  oImageCreateInfo.usage = _uUsage;
  oImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  oImageCreateInfo.samples = _eMsaaSampleCount;
  oImageCreateInfo.flags = _uCreateFlags;

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

void CreateImageView(const APIWindow* _pWindow
  , VkImage _hImage
  , VkFormat _eFormat
  , uint32_t _uBaseMip
  , uint32_t _uMipLevels  
  , uint32_t _uBaseLayer
  , uint32_t _uLayers
  , VkImageViewType _eViewType
  , VkImageAspectFlags _uAspectFlags
  , VkImageView& hImageView_)
{
  VkImageViewCreateInfo oCreateInfo = {};
  oCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  oCreateInfo.pNext = NULL;
  oCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  oCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  oCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  oCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  oCreateInfo.subresourceRange.aspectMask = _uAspectFlags;
  oCreateInfo.subresourceRange.baseMipLevel = _uBaseMip;
  oCreateInfo.subresourceRange.levelCount = _uMipLevels;
  oCreateInfo.subresourceRange.baseArrayLayer = _uBaseLayer;
  oCreateInfo.subresourceRange.layerCount = _uLayers;
  oCreateInfo.image = _hImage;
  oCreateInfo.viewType = _eViewType;
  oCreateInfo.format = _eFormat;

  VK_CHECK(vkCreateImageView(_pWindow->m_hDevice, &oCreateInfo, NULL, &hImageView_))
}

void CreateTextureSampler(const APIWindow* _pWindow, APITexture* _pTexture, uint32_t _uMipLevels, const SamplerConfig& _rSamplerConfig)
{

  VkPhysicalDeviceProperties oProperties{};
  vkGetPhysicalDeviceProperties(s_oGlobalData.m_hPhysicalDevice, &oProperties);

  VkSamplerAddressMode eAddressMode = GetVkAddressMode(_rSamplerConfig.m_eAddressMode);

  VkSamplerCreateInfo oSamplerInfo{};
  oSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  oSamplerInfo.addressModeU = eAddressMode;
  oSamplerInfo.addressModeV = eAddressMode;
  oSamplerInfo.addressModeW = eAddressMode;
  oSamplerInfo.anisotropyEnable = VK_TRUE;
  oSamplerInfo.maxAnisotropy = oProperties.limits.maxSamplerAnisotropy;
  oSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  oSamplerInfo.unnormalizedCoordinates = VK_FALSE;
  oSamplerInfo.compareEnable = VK_FALSE;
  oSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  oSamplerInfo.mipmapMode = GetSamplerMipmapMode(_rSamplerConfig.m_eMipmapFilterMode);
  oSamplerInfo.minLod = 0.0f;
  oSamplerInfo.maxLod = static_cast<float>(_uMipLevels);
  oSamplerInfo.mipLodBias = 0.0f;
  oSamplerInfo.magFilter = GetVkFilter(_rSamplerConfig.m_eMagFilterMode);
  oSamplerInfo.minFilter = GetVkFilter(_rSamplerConfig.m_eMinFilterMode);

  VK_CHECK(vkCreateSampler(_pWindow->m_hDevice, &oSamplerInfo, NULL, &_pTexture->m_hSampler))
}

void DestroyBuffer(const APIWindow* _pWindow, VkBuffer _hBuffer, VkDeviceMemory _hDeviceMemory_)
{
  vkDestroyBuffer(_pWindow->m_hDevice, _hBuffer, NULL);
  vkFreeMemory(_pWindow->m_hDevice, _hDeviceMemory_, NULL);
}

void SetBufferData(const APIWindow* _pWindow, const void* _pData, size_t _uSize, VkDeviceMemory& hDeviceMemory_)
{
  VkDevice hDevice = _pWindow->m_hDevice;
  void* pDeviceData;
  VK_CHECK(vkMapMemory(hDevice, hDeviceMemory_, 0, _uSize, 0, &pDeviceData))
    memcpy(pDeviceData, _pData, _uSize);
  vkUnmapMemory(hDevice, hDeviceMemory_);
}

VkCommandBuffer BeginTempCmdBuffer(const APIWindow* _pWindow)
{

  VkCommandBufferAllocateInfo oAllocInfo{};
  oAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  oAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  oAllocInfo.commandPool = _pWindow->m_hCmdPool;
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

void EndTempCmdBuffer(const APIWindow* _pWindow, VkCommandBuffer _hCmdBuffer)
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

  vkFreeCommandBuffers(_pWindow->m_hDevice, _pWindow->m_hCmdPool, 1u, &_hCmdBuffer);
}

void CopyBuffer(VkCommandBuffer _hCmdBuffer, VkBuffer _hSrcBuffer, VkBuffer _hDstBuffer, VkDeviceSize _uSize)
{
  VkBufferCopy oCopyRegion{};
  oCopyRegion.srcOffset = 0; // Optional
  oCopyRegion.dstOffset = 0; // Optional
  oCopyRegion.size = _uSize;
  vkCmdCopyBuffer(_hCmdBuffer, _hSrcBuffer, _hDstBuffer, 1, &oCopyRegion);
}

void CopyBufferToImage(VkCommandBuffer _hCmdBuffer, VkBuffer _hBuffer, VkImage _hImage, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uLayers)
{  
  VkBufferImageCopy oCopyRegion{};
  oCopyRegion.bufferOffset = 0;
  oCopyRegion.bufferRowLength = 0;
  oCopyRegion.bufferImageHeight = 0;
  oCopyRegion.imageOffset = { 0, 0, 0 };
  oCopyRegion.imageExtent = { _uWidth, _uHeight, 1 };
  oCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  oCopyRegion.imageSubresource.baseArrayLayer = 0;
  oCopyRegion.imageSubresource.layerCount = _uLayers;
  oCopyRegion.imageSubresource.mipLevel = 0;

  vkCmdCopyBufferToImage(_hCmdBuffer, _hBuffer, _hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &oCopyRegion);
}

void GetLayoutAccessAndStage(VkImageLayout _eLayout, VkAccessFlags& uAccessMask_, VkPipelineStageFlags& uStageMask_)
{
  switch (_eLayout)
  {
  case VK_IMAGE_LAYOUT_UNDEFINED:
    uAccessMask_ = 0;
    uStageMask_ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    uAccessMask_ = VK_ACCESS_TRANSFER_WRITE_BIT;
    uStageMask_ = VK_PIPELINE_STAGE_TRANSFER_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    uAccessMask_ = VK_ACCESS_TRANSFER_READ_BIT;
    uStageMask_ = VK_PIPELINE_STAGE_TRANSFER_BIT;
    break;
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    uAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    uStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    uAccessMask_ = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    uStageMask_ = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    break;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    uAccessMask_ = VK_ACCESS_SHADER_READ_BIT;
    uStageMask_ = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    break;
  case VK_IMAGE_LAYOUT_GENERAL:
    uAccessMask_ = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    uStageMask_ = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    break;
  case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    uAccessMask_ = 0;
    uStageMask_ = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    break;
  default:
    THROW_GENERIC_EXCEPTION("[API] Unsupported layout in transition");
    break;
  }
}

void TransitionImageLayout(VkCommandBuffer _hCmdBuffer
  , VkImage _hImage
  , VkFormat _eFormat
  , uint32_t _uBaseMip  
  , uint32_t _uMipLevels  
  , uint32_t _uBaseLayer
  , uint32_t _uLayers
  , VkImageAspectFlags _uAspectFlags
  , VkImageLayout _eOldLayout
  , VkImageLayout _eNewLayout)
{  

  VkImageMemoryBarrier oBarrier{};
  oBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  oBarrier.oldLayout = _eOldLayout;
  oBarrier.newLayout = _eNewLayout;
  oBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  oBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  oBarrier.image = _hImage;
  oBarrier.subresourceRange.aspectMask = _uAspectFlags;
  oBarrier.subresourceRange.baseMipLevel = _uBaseMip;
  oBarrier.subresourceRange.levelCount = _uMipLevels;
  oBarrier.subresourceRange.baseArrayLayer = _uBaseLayer;
  oBarrier.subresourceRange.layerCount = _uLayers;

  VkPipelineStageFlags uSrcStage;
  VkPipelineStageFlags uDstStage;

  GetLayoutAccessAndStage(_eOldLayout, oBarrier.srcAccessMask, uSrcStage);
  GetLayoutAccessAndStage(_eNewLayout, oBarrier.dstAccessMask, uDstStage);

  vkCmdPipelineBarrier(_hCmdBuffer, uSrcStage, uDstStage, 0, 0, NULL, 0, NULL, 1, &oBarrier);  
}

void TransitionTextureImageLayout(VkCommandBuffer _hCmdBuffer
  , APITexture* _pTexture
  , uint32_t _uBaseMip
  , uint32_t _uMipLevels
  , uint32_t _uBaseLayer
  , uint32_t _uLayers
  , VkImageAspectFlags _uAspectFlags
  , VkImageLayout _eNewLayout
  , bool _bForce)
{  
  // The top mip level + 1
  uint32_t uEndMipLevel = _uBaseMip + _uMipLevels;

  // Accumulate layers until the layout is different or last layer reached 
  uint32_t uBaseMip = _uBaseMip;
  for (uint32_t i = _uBaseMip; i <= uEndMipLevel; i++)
  {     
    // clamp index
    uint32_t uIdx = i >= uEndMipLevel ? (uEndMipLevel - 1) : i;

    if (_pTexture->m_aCurrLayouts[uBaseMip] != _pTexture->m_aCurrLayouts[uIdx]
      || i >= uEndMipLevel)
    {
      if (_pTexture->m_aCurrLayouts[uBaseMip] != _eNewLayout)
      {
        TransitionImageLayout(_hCmdBuffer
          , _pTexture->m_hImage
          , _pTexture->m_eFormat          
          , uBaseMip, (i - uBaseMip)
          , _uBaseLayer, _uLayers
          , _uAspectFlags
          , _pTexture->m_aCurrLayouts[uBaseMip], _eNewLayout);

        for(uint32_t j = uBaseMip; j <= i; j++)
        {
          _pTexture->m_aCurrLayouts[j] = _eNewLayout; 
        }
      }
      uBaseMip = i;
    }
  }
}

void SetTextureLayoutAllMipLevels(APITexture* _pTexture, VkImageLayout _eNewLayout)
{
  for (uint32_t i = 0; i < _pTexture->m_uMipLevels; i++)
  {
    _pTexture->m_aCurrLayouts[i] = _eNewLayout;
  }
}

void GenerateMipmaps(VkCommandBuffer _hCmdBuffer, VkImage _hImage, int32_t _iWidth, int32_t _iHeight, uint32_t _uMipLevels, uint32_t _uLayers)
{
  VkImageMemoryBarrier oBarrier{};
  oBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  oBarrier.image = _hImage;
  oBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  oBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  oBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  oBarrier.subresourceRange.baseArrayLayer = 0;
  oBarrier.subresourceRange.layerCount = _uLayers;
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

    vkCmdPipelineBarrier(_hCmdBuffer,
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
    oBlit.srcSubresource.layerCount = _uLayers;
    oBlit.dstOffsets[0] = { 0, 0, 0 };
    oBlit.dstOffsets[1] = { iMipWidth > 1 ? iMipWidth / 2 : 1, iMipHeight > 1 ? iMipHeight / 2 : 1, 1 };
    oBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    oBlit.dstSubresource.mipLevel = i;
    oBlit.dstSubresource.baseArrayLayer = 0;
    oBlit.dstSubresource.layerCount = _uLayers;

    vkCmdBlitImage(_hCmdBuffer,
      _hImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      _hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1u, &oBlit,
      VK_FILTER_LINEAR);

    oBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    oBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;;
    oBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    oBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(_hCmdBuffer,
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

  vkCmdPipelineBarrier(_hCmdBuffer,
    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
    0, nullptr,
    0, nullptr,
    1, &oBarrier);
}

void GetDescSetReflection(const APIRenderState* _pRenderState, PipelineStage _eStage, SpvReflectDescriptorSet* aDescSets_[4], uint32_t& uDescSetCount_)
{
  const SpvReflectShaderModule* pReflection = nullptr;

  switch (_eStage)
  {
  case STAGE_VERTEX:
    pReflection = &_pRenderState->m_oVertexReflection;
    break;
  case STAGE_GEOM:
    pReflection = &_pRenderState->m_oGeomReflection;
    break;
  case STAGE_PIXEL:
    pReflection = &_pRenderState->m_oPixelReflection;
    break;
  }

  uDescSetCount_ = 0u;

  if (pReflection->entry_point_name != nullptr)
  {
    SpvReflectResult eResult = spvReflectEnumerateEntryPointDescriptorSets(pReflection, "main", &uDescSetCount_, NULL);
    if (eResult != SPV_REFLECT_RESULT_SUCCESS)
    {
      THROW_GENERIC_EXCEPTION("[API] Error: Failed to enumerate descriptor sets from shader: ")
    }
    if (uDescSetCount_ > 4u)
    {
      THROW_GENERIC_EXCEPTION("[API] Error: the shader has more than 4 descriptor sets : ")
    }

    eResult = spvReflectEnumerateEntryPointDescriptorSets(pReflection, "main", &uDescSetCount_, aDescSets_);
    if (eResult != SPV_REFLECT_RESULT_SUCCESS)
    {
      THROW_GENERIC_EXCEPTION("[API] Error: Failed to get descriptor sets from shader")
    }
  }  
}

void AddBufferToDescSetUpdater(const APIWindow* _pWindow, APIConstantBuffer* _pCBuffer, size_t _uSize, uint32_t uBinding, PipelineStageFlags _uStageFlags)
{
  if (s_oGlobalData.m_bRenderBegan)
  {
    VkDescriptorBufferInfo oBufferInfo{};
    oBufferInfo.buffer = _pCBuffer->m_pUniformBuffers[_pWindow->m_uCurrFrameIdx];
    oBufferInfo.offset = 0;
    oBufferInfo.range = _uSize;

    s_oGlobalData.m_oDescSetUpdater.AddBufferInfo(std::move(oBufferInfo), uBinding, _pWindow->m_uCurrFrameIdx, _uStageFlags);
  }
  else
  {   
    for (int i = 0; i < _pWindow->m_uSwapchainImageCount; i++)
    {
      VkDescriptorBufferInfo oBufferInfo{};
      oBufferInfo.buffer = _pCBuffer->m_pUniformBuffers[i];
      oBufferInfo.offset = 0;
      oBufferInfo.range = _uSize;

      s_oGlobalData.m_oDescSetUpdater.AddBufferInfo(std::move(oBufferInfo), uBinding, i, _uStageFlags);
    }
  }
}

void AddImageToDescSetUpdater(const APIWindow* _pWindow, APITexture* _pTexture, uint32_t uBinding, PipelineStageFlags _uStageFlags)
{  
  if (s_oGlobalData.m_bRenderBegan)
  {
    VkDescriptorImageInfo oImageInfo{};
    oImageInfo.sampler = _pTexture->m_hSampler;
    oImageInfo.imageView = _pTexture->m_hImageView;
    oImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    s_oGlobalData.m_oDescSetUpdater.AddImageInfo(std::move(oImageInfo), uBinding, _pWindow->m_uCurrFrameIdx, _uStageFlags);
  }
  else
  {
    for (int i = 0; i < _pWindow->m_uSwapchainImageCount; i++)
    {
      VkDescriptorImageInfo oImageInfo{};
      oImageInfo.sampler = _pTexture->m_hSampler;
      oImageInfo.imageView = _pTexture->m_hImageView;
      oImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      s_oGlobalData.m_oDescSetUpdater.AddImageInfo(std::move(oImageInfo), uBinding, i, _uStageFlags);
    }
  }
}

static void check_vk_result(VkResult err)
{
  if (err == 0)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}

void SetupImGui(const APIWindow* _pWindow)
{
  ImGui_ImplGlfw_InitForVulkan(_pWindow->m_pGlfwWindow, true);

  VkSurfaceCapabilitiesKHR oSurfaceCapabilities;
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_oGlobalData.m_hPhysicalDevice, _pWindow->m_hSurface, &oSurfaceCapabilities))

  ImGui_ImplVulkan_InitInfo oImGuiInfo = {};
  oImGuiInfo.Instance = s_oGlobalData.m_hInstance;
  oImGuiInfo.PhysicalDevice = s_oGlobalData.m_hPhysicalDevice;
  oImGuiInfo.Device = _pWindow->m_hDevice;
  oImGuiInfo.QueueFamily = s_oGlobalData.m_uRenderQueueFamilyIdx;
  oImGuiInfo.Queue = _pWindow->m_hRenderQueue;
  oImGuiInfo.PipelineCache = VK_NULL_HANDLE;
  oImGuiInfo.DescriptorPool = _pWindow->m_hDescPool;
  oImGuiInfo.Subpass = 0;
  oImGuiInfo.MinImageCount = oSurfaceCapabilities.minImageCount;
  oImGuiInfo.ImageCount = _pWindow->m_uSwapchainImageCount;
  oImGuiInfo.MSAASamples = _pWindow->m_eMsaaSamples;
  oImGuiInfo.Allocator = nullptr;
  oImGuiInfo.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&oImGuiInfo, _pWindow->m_hRenderPass);

  VkCommandBuffer hCmdBuffer = BeginTempCmdBuffer(_pWindow);
  ImGui_ImplVulkan_CreateFontsTexture(hCmdBuffer);
  EndTempCmdBuffer(_pWindow, hCmdBuffer);

  ImGui_ImplVulkan_DestroyFontUploadObjects();
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

} // namespace vk
} // namespace api