#pragma once

#include <array>
#include <vulkan/vulkan.h>

#include "Graphics/Vertex.h"

namespace api
{
namespace vk
{

  struct APIWindow;

  struct APIMesh
  {
  public:

    APIWindow* m_pOwnerWindow;    
    VkBuffer m_hVertexBuffer;    
    VkDeviceMemory m_hVertexBufferMemory;    
    VkBuffer m_hIndexBuffer;
    VkDeviceMemory m_hIndexBufferMemory;

    static VkVertexInputBindingDescription GetBindingDesc()
    {
      VkVertexInputBindingDescription oBindingDesc{};

      oBindingDesc.binding = 0;
      oBindingDesc.stride = sizeof(Vertex);
      oBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return oBindingDesc;
    }

    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDesc()
    {
      std::array<VkVertexInputAttributeDescription, 4> oAttributeDesc{};

      oAttributeDesc[0].binding = 0;
      oAttributeDesc[0].location = 0;
      oAttributeDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      oAttributeDesc[0].offset = offsetof(Vertex, m_vPos);

      oAttributeDesc[1].binding = 0;
      oAttributeDesc[1].location = 1;
      oAttributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      oAttributeDesc[1].offset = offsetof(Vertex, m_vNormal);

      oAttributeDesc[2].binding = 0;
      oAttributeDesc[2].location = 2;
      oAttributeDesc[2].format = VK_FORMAT_R32G32B32_SFLOAT;
      oAttributeDesc[2].offset = offsetof(Vertex, m_vColor);

      oAttributeDesc[3].binding = 0;
      oAttributeDesc[3].location = 3;
      oAttributeDesc[3].format = VK_FORMAT_R32G32_SFLOAT;
      oAttributeDesc[3].offset = offsetof(Vertex, m_vUv);

      return oAttributeDesc;
    }

  };

}
}