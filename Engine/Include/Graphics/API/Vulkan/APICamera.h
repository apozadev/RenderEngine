#pragma once

#include <vulkan/vulkan.h>

namespace api
{
  namespace vk
  {

    struct APIWindow;

    struct APICamera
    {      
      VkDescriptorSet* m_pDescSets;

      APIWindow* m_pOwnerWindow;
    };

  }
}