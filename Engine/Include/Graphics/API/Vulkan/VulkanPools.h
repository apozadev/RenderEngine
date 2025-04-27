#pragma once

#include "Memory/TypedPool.h"
#include "Memory/TypedContiguousPool.h"

#include "Graphics/API/Vulkan/APIRenderState.h"
#include "Graphics/API/Vulkan/APIRenderSubState.h"
#include "Graphics/API/Vulkan/APIConstantBuffer.h"
#include "Graphics/API/Vulkan/APIMesh.h"
#include "Graphics/API/Vulkan/APITexture.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"

#include "../3rd/spirv-reflect/spirv_reflect.h"

namespace api
{
namespace vk
{

  extern TypedPool<APIRenderState, 256>     s_oRenderStatePool;
  extern TypedPool<APIRenderSubState, 256>  s_oRenderSubStatePool;
  extern TypedPool<APIConstantBuffer, 256>  s_oConstantBufferPool;
  extern TypedPool<APIMesh, 256>            s_oMeshPool;
  extern TypedPool<APITexture, 256>         s_oTexturePool;
  extern TypedPool<APIRenderTarget, 256>    s_oRenderTargetPool;

  extern TypedContiguousPool<VkBuffer, 256>               s_oVkBufferPool;
  extern TypedContiguousPool<VkDeviceMemory, 256>         s_oVkDeviceMemoryPool;
  extern TypedContiguousPool<void*, 256>                  s_oVoidPtrPool;
  extern TypedContiguousPool<VkDescriptorSet, 256>        s_oVkDescriptorSetPool;  
  extern TypedContiguousPool<VkAttachmentReference, 3>    s_oVkAttachmentReferencePool;
  extern TypedContiguousPool<VkAttachmentDescription, 3>  s_oVkAttachmentDescriptionPool;
  extern TypedContiguousPool<VkImageView, 256>            s_oVkImageViewPool;
}
}
