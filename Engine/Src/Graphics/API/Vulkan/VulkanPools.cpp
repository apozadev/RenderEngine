#include "Graphics/API/Vulkan/VulkanPools.h"

namespace api
{
namespace vk
{

  TypedPool<APIRenderState, 256>     s_oRenderStatePool;
  TypedPool<APIRenderSubState, 256>  s_oRenderSubStatePool;
  TypedPool<APIConstantBuffer, 256>  s_oConstantBufferPool;
  TypedPool<APIMesh, 256>            s_oMeshPool;
  TypedPool<APITexture, 256>         s_oTexturePool;
  TypedPool<APIRenderTarget, 256>    s_oRenderTargetPool; 

  TypedContiguousPool<VkBuffer, 256>               s_oVkBufferPool;
  TypedContiguousPool<VkDeviceMemory, 256>         s_oVkDeviceMemoryPool;
  TypedContiguousPool<void*, 256>                  s_oVoidPtrPool;
  TypedContiguousPool<VkDescriptorSet, 256>        s_oVkDescriptorSetPool;
  TypedContiguousPool<VkAttachmentReference, 3>    s_oVkAttachmentReferencePool;
  TypedContiguousPool<VkAttachmentDescription, 3>  s_oVkAttachmentDescriptionPool;
  TypedContiguousPool<VkImageView, 256>            s_oVkImageViewPool;

}
}