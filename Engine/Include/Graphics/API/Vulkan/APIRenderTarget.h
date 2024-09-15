#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

  struct APIWindow;

  struct APIRenderTarget
  {
    VkRenderPass m_hRenderPass;
    VkFramebuffer m_hFrameBuffer;

    APIWindow* m_pOwnerWindow;
  };

}
}