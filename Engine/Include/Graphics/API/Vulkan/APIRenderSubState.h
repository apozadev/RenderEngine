#pragma once

#include <vulkan/vulkan.h>

namespace api
{
  namespace vk
  {

    struct APIRenderState;    

    struct APIRenderSubState
    {
    public:      
      
      VkDescriptorSet* m_pDescSets;

      APIRenderState* m_pRenderState;
    };

  }
}
