#pragma once

#include <vulkan/vulkan.h>

namespace api
{
  namespace vk
  {

    struct APIWindow;

    struct APICamera
    {      
      APIWindow* m_pOwnerWindow;
    };

  }
}