#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../3rd/spirv-reflect/spirv_reflect.h"

#include "File/InFile.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/PipelineStage.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/CullMode.h"
#include "Graphics/DepthCompareOp.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/ResourceBindInfo.h"
#include "Graphics/SamplerConfig.h"
#include "Graphics/API/Vulkan/VulkanData.h"
#include "Graphics/API/Vulkan/APIWindow.h"
#include "Graphics/API/Vulkan/APICamera.h"
#include "Graphics/API/Vulkan/APIMesh.h"
#include "Graphics/API/Vulkan/APIConstantBuffer.h"
#include "Graphics/API/Vulkan/APIRenderState.h"
#include "Graphics/API/Vulkan/APIRenderSubState.h"
#include "Graphics/API/Vulkan/APITexture.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"
#include "Graphics/API/Vulkan/DescriptorUtils.h"

struct GLFWwindow;

namespace api
{
namespace vk
{

  extern VulkanData s_oGlobalData;

  VkFormat GetVKFormat(ImageFormat _eFormat);

  VkShaderStageFlags GetVkShaderFlags(PipelineStageFlags _eStageFlags);

  VkBlendOp GetVkBlendOp(BlendOp _eBlendOp);

  VkBlendFactor GetVkBlendFactor(BlendFactor _eBlendFactor);

  VkImageUsageFlags GetVkTextureUsage(uint32_t _uUsage);

  VkSamplerAddressMode GetVkAddressMode(TextureAddressMode _AddressMode);

  VkSamplerMipmapMode GetSamplerMipmapMode(TextureFilterMode _eMipmapMode);

  VkFilter GetVkFilter(TextureFilterMode _eFliterMode);

  VkCullModeFlagBits GetVkCullMode(CullMode _eCullMode);

  VkCompareOp GetVkCompareOp(DepthCompareOp _eCompareOp);

  void CreateInstance();

  VkSampleCountFlagBits GetMaxMSAASampleCount();

  uint32_t FindBinding(const APIWindow* _pWindow, const ResourceBindInfo& _oBindInfo, VkDescriptorType _eType);

  void CreatePhysicalDevice();

  void CreatePipeline(const file::InFile& _oVSFile, const file::InFile& _oPSFile, const file::InFile* _pGSFile, const RenderStateInfo& _oInfo, VkRenderPass _hRenderPass, VkSampleCountFlagBits _uMsaaSamples, APIRenderState* _pRenderState_);

  void CreateLogicalDevice(APIWindow* _pWindow);

  void RetrieveQueues(APIWindow* _pWindow);

  void CreateSurface(APIWindow* _pWindow, GLFWwindow* _pGlfwWindow);

  void CreateColorBuffer(APIWindow* _pWindow);

  void CreateDepthBuffer(APIWindow* _pWindow);

  void CreateSwapchain(APIWindow* _pWindow);

  void CreateRenderPass(APIWindow* _pWindow, uint32_t _uNumColorTextures, VkFormat _eColorFormat, bool _bHasDepthStencil, VkFormat _eDepthStencilFormat, uint32_t _uMsaaSampleCount, VkRenderPass& hRenderPass_, bool _bOffscreen, bool _bMultiview = false);  

  void CreateCommandBuffers(APIWindow* _pWindow);

  void CreateSyncObjects(APIWindow* _pWindow);

  void CreateDescriptorPool(APIWindow* _pWindow);

  void CreateGlobalDescriptorLayout(APIWindow* _pWindow);

  void DestroySwapchain(APIWindow* _pWindow);

  void RecreateSwapchain(APIWindow* _pWindow);

  uint32_t FindMemoryType(uint32_t _uTypeFilter, VkMemoryPropertyFlags _uProperties);

  void CreateBuffer(APIWindow* _pWindow, size_t _uSize, VkBufferUsageFlags _uUsage, VkMemoryPropertyFlags _uProperties, VkBuffer& hBuffer_, VkDeviceMemory& hDeviceMemory_);

  void CreateImage(APIWindow* _pWindow, uint32_t _uWidth, uint32_t _uHeight, VkFormat _eFormat, uint32_t _uMipLevels, uint32_t _uLayers, VkSampleCountFlagBits _eMsaaSampleCount, VkImageTiling _eTiling, VkImageUsageFlags _uUsage, VkImageCreateFlags _uCreateFlags, VkMemoryPropertyFlags _uProperties, VkImage& hImage_, VkDeviceMemory& hMemory_);

  void CreateImageView(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, uint32_t _uLayers, VkImageViewType _eViewType, VkImageAspectFlags _uAspectFlags, VkImageView& hImageView_);

  void CreateTextureSampler(APIWindow* _pWindow, APITexture* _pTexture, uint32_t _uMipLevels, const SamplerConfig& _rSamplerConfig);

  void DestroyBuffer(APIWindow* _pWindow, VkBuffer _hBuffer, VkDeviceMemory _hDeviceMemory_);

  void SetBufferData(APIWindow* _pWindow, const void* _pData, size_t _uSize, VkDeviceMemory& hDeviceMemory_);

  VkCommandBuffer BeginTempCmdBuffer(APIWindow* _pWindow);

  void EndTempCmdBuffer(APIWindow* _pWindow, VkCommandBuffer _hCmdBuffer);

  void CopyBuffer(APIWindow* _pWindow, VkBuffer _hSrcBuffer, VkBuffer _hDstBuffer, VkDeviceSize _uSize);

  void CopyBufferToImage(APIWindow* _pWindow, VkBuffer _hBuffer, VkImage _hImage, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uLayers);

  void TransitionImageLayout(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, uint32_t _uLayers, VkImageAspectFlags _uAspectFlags, VkImageLayout _eOldLayout, VkImageLayout _eNewLayout);

  void TransitionImageLayoutOffline(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, uint32_t _uLayers, VkImageAspectFlags _uAspectFlags, VkImageLayout _eOldLayout, VkImageLayout _eNewLayout);

  void GenerateMipmaps(APIWindow* _pWindow, VkImage _hImage, int32_t _iWidth, int32_t _iHeight, uint32_t _uMipLevels, uint32_t _uLayers);

  void GetDescSetReflection(const APIRenderState* _pRenderState, PipelineStage _eStage, SpvReflectDescriptorSet* aDescSets_[4], uint32_t& uDescSetCount_);

  void SetupImGui(APIWindow* _pWindow);

} // namespace vk
} // namespace api