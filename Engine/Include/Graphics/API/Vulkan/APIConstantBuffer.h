#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIConstantBuffer
  {
  public:
    VkBuffer* m_pUniformBuffers;
    VkDeviceMemory* m_pUniformBuffersMemory;
    void** m_pUniformBuffersMapped;    
  };

}
}