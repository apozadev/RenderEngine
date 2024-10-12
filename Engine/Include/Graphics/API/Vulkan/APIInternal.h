#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "File/File.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/PipelineStage.h"
#include "Graphics/BlendEnums.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/API/Vulkan/VulkanData.h"
#include "Graphics/API/Vulkan/APIWindow.h"
#include "Graphics/API/Vulkan/APICamera.h"
#include "Graphics/API/Vulkan/APIMesh.h"
#include "Graphics/API/Vulkan/APIConstantBuffer.h"
#include "Graphics/API/Vulkan/APIRenderState.h"
#include "Graphics/API/Vulkan/APIRenderSubState.h"
#include "Graphics/API/Vulkan/APITexture.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"

struct GLFWwindow;

namespace api
{
namespace vk
{

  extern VulkanData s_oGlobalData;

  VkFormat GetVKFormat(ImageFormat _eFormat);

  VkShaderStageFlagBits GetVkStageFlag(PipelineStage _eStage);

  VkBlendOp GetVkBlendOp(BlendOp _eBlendOp);

  VkBlendFactor GetVkBlendFactor(BlendFactor _eBlendFactor);

  VkImageUsageFlags GetVkTextureUsage(uint32_t _uUsage);

  void CreateInstance();

  VkSampleCountFlagBits GetMaxMSAASampleCount();

  void CreatePhysicalDevice();

  void CreatePipeline(const file::File& _oVSFile, const file::File& _oPSFile, const RenderStateInfo& _oInfo, VkRenderPass _hRenderPass, VkSampleCountFlagBits _uMsaaSamples, APIRenderState* _pRenderState_);

  void CreateLogicalDevice(APIWindow* _pWindow);

  void RetrieveQueues(APIWindow* _pWindow);

  void CreateSurface(APIWindow* _pWindow, GLFWwindow* _pGlfwWindow);

  void CreateColorBuffer(APIWindow* _pWindow);

  void CreateDepthBuffer(APIWindow* _pWindow);

  void CreateSwapchain(APIWindow* _pWindow);

  void CreateRenderPass(APIWindow* _pWindow, uint32_t _uNumColorTextures, VkFormat _eColorFormat, bool _bHasDepthStencil, VkFormat _eDepthStencilFormat, uint32_t _uMsaaSampleCount, VkRenderPass& hRenderPass_, bool _bOffscreen);

  void CreateFramebuffer(APIWindow* _pWindow, VkRenderPass _hRenderPass, APITexture** _pColorTextures, uint32_t _uNumColorTextures, APITexture* _pDepthTexture, APITexture** _pColorResolveTextures, VkFramebuffer& hFrameBuffer_);

  void CreateFramebuffer(APIWindow* _pWindow, VkRenderPass _hRenderPass, VkImageView _hColorImageView, VkImageView _hDepthImageView, VkImageView _hResolveImageView, VkFramebuffer& hFrameBuffer_);

  void CreateCommandBuffers(APIWindow* _pWindow);

  void CreateSyncObjects(APIWindow* _pWindow);

  void CreateDescriptorPool(APIWindow* _pWindow);

  void CreateGlobalDescriptorLayout(APIWindow* _pWindow);

  void DestroySwapchain(APIWindow* _pWindow);

  void RecreateSwapchain(APIWindow* _pWindow);

  uint32_t FindMemoryType(uint32_t _uTypeFilter, VkMemoryPropertyFlags _uProperties);

  void CreateBuffer(APIWindow* _pWindow, size_t _uSize, VkBufferUsageFlags _uUsage, VkMemoryPropertyFlags _uProperties, VkBuffer& hBuffer_, VkDeviceMemory& hDeviceMemory_);

  void CreateImage(APIWindow* _pWindow, uint32_t _uWidth, uint32_t _uHeight, VkFormat _eFormat, uint32_t _uMipLevels, VkSampleCountFlagBits _eMsaaSampleCount, VkImageTiling _eTiling, VkImageUsageFlags _uUsage, VkMemoryPropertyFlags _uProperties, VkImage& hImage_, VkDeviceMemory& hMemory_);

  void CreateImageView(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, VkImageAspectFlags _uAspectFlags, VkImageView& hImageView_);

  void CreateTextureSampler(APIWindow* _pWindow, APITexture* _pTexture, uint32_t _uMipLevels);

  void DestroyBuffer(APIWindow* _pWindow, VkBuffer _hBuffer, VkDeviceMemory _hDeviceMemory_);

  void SetBufferData(APIWindow* _pWindow, const void* _pData, size_t _uSize, VkDeviceMemory& hDeviceMemory_);

  VkCommandBuffer BeginTempCmdBuffer(APIWindow* _pWindow);

  void EndTempCmdBuffer(APIWindow* _pWindow, VkCommandBuffer _hCmdBuffer);

  void CopyBuffer(APIWindow* _pWindow, VkBuffer _hSrcBuffer, VkBuffer _hDstBuffer, VkDeviceSize _uSize);

  void CopyBufferToImage(APIWindow* _pWindow, VkBuffer _hBuffer, VkImage _hImage, uint32_t _uWidth, uint32_t _uHeight);

  void TransitionImageLayout(APIWindow* _pWindow, VkImage _hImage, VkFormat _eFormat, uint32_t _uMipLevels, VkImageAspectFlags _uAspectFlags, VkImageLayout _eOldLayout, VkImageLayout _eNewLayout);

  void GenerateMipmaps(APIWindow* _pWindow, VkImage _hImage, int32_t _iWidth, int32_t _iHeight, uint32_t _uMipLevels);

} // namespace vk
} // namespace api