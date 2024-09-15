#pragma once

#include <vulkan/vulkan.h>

namespace api
{
  namespace vk
  {

    struct APIRenderState;    
    struct APIWindow;    

    struct APIRenderSubState
    {
    public:      
      
      VkDescriptorSet* m_pDescSets;

      VkDescriptorSetLayout m_hDecSetLayout;

      APIWindow* m_pOwnerWindow;
    };

  }
}
