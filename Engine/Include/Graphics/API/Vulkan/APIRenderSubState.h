#pragma once

#include <vulkan/vulkan.h>

namespace api
{
  namespace vk
  {   

    struct APIRenderSubState
    {
    public:      
      
      VkDescriptorSet* m_pDescSets;

      VkDescriptorSetLayout m_hDecSetLayout;
    };

  }
}
