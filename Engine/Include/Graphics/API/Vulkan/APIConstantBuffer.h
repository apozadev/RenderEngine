#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIRenderState;

  struct APIConstantBuffer
  {
  public:
    VkBuffer* m_pUniformBuffers;
    VkDeviceMemory* m_pUniformBuffersMemory;
    void** m_pUniformBuffersMapped;

    VkDescriptorPool m_hDescPool;
    VkDescriptorSet* m_pDescSets;

    APIRenderState* m_pOwnerRenderState;
  };

}
}