#include "Graphics/API/Vulkan/VulkanPools.h"

namespace api
{
namespace vk
{

  TypedPool<APIRenderState>     s_oRenderStatePool;
  TypedPool<APIRenderSubState>  s_oRenderSubStatePool;
  TypedPool<APIConstantBuffer>  s_oConstantBufferPool;
  TypedPool<APIMesh>            s_oMeshPool;
  TypedPool<APITexture>         s_oTexturePool;
  TypedPool<APIRenderTarget>    s_oRenderTargetPool; 

  TypedContiguousPool<VkBuffer>                 s_oVkBufferPool;
  TypedContiguousPool<VkDeviceMemory>           s_oVkDeviceMemoryPool;
  TypedContiguousPool<void*>                    s_oVoidPtrPool;
  TypedContiguousPool<VkDescriptorSet>          s_oVkDescriptorSetPool;
  TypedContiguousPool<VkAttachmentReference>    s_oVkAttachmentReferencePool;
  TypedContiguousPool<VkAttachmentDescription>  s_oVkAttachmentDescriptionPool;

}
}