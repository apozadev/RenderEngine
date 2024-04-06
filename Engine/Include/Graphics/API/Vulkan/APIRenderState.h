#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{

struct APIWindow;

struct APIRenderState
{
public:
  VkPipelineLayout m_hPipelineLayout;
  VkDescriptorSetLayout m_hDescSetLayout;  
  VkPipeline m_hGraphicsPipeline;
  APIWindow* m_pOwnerWindow;
};

}
}
