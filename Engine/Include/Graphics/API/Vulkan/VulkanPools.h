#pragma once

#include "Core/TypedPool.h"
#include "Core/TypedContiguousPool.h"

#include "Graphics/API/Vulkan/APIRenderState.h"
#include "Graphics/API/Vulkan/APIRenderSubState.h"
#include "Graphics/API/Vulkan/APIConstantBuffer.h"
#include "Graphics/API/Vulkan/APIMesh.h"
#include "Graphics/API/Vulkan/APITexture.h"
#include "Graphics/API/Vulkan/APIRenderTarget.h"

namespace api
{
namespace vk
{

  extern TypedPool<APIRenderState>      s_oRenderStatePool;
  extern TypedPool<APIRenderSubState>   s_oRenderSubStatePool;
  extern TypedPool<APIConstantBuffer>   s_oConstantBufferPool;
  extern TypedPool<APIMesh>             s_oMeshPool;
  extern TypedPool<APITexture>          s_oTexturePool;
  extern TypedPool<APIRenderTarget>     s_oRenderTargetPool;

  extern TypedContiguousPool<VkBuffer>                s_oVkBufferPool;
  extern TypedContiguousPool<VkDeviceMemory>          s_oVkDeviceMemoryPool;
  extern TypedContiguousPool<void*>                   s_oVoidPtrPool;
  extern TypedContiguousPool<VkDescriptorSet>         s_oVkDescriptorSetPool;  
  extern TypedContiguousPool<VkAttachmentReference>   s_oVkAttachmentReferencePool;
  extern TypedContiguousPool<VkAttachmentDescription> s_oVkAttachmentDescriptionPool;

}
}
