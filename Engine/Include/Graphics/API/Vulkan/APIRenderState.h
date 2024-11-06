#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/API/Vulkan/APIRenderSubState.h"
#include "Graphics/API/Vulkan/DescriptorUtils.h"

namespace api
{
namespace vk
{

struct APIWindow;

struct APIRenderState
{
public:
  VkPipelineLayout m_hPipelineLayout;  
  VkPipeline m_hGraphicsPipeline;  

  VkDescriptorSet* m_pDescSets;

  VkDescriptorSetLayout m_hDescSetLayout;  

  APIWindow* m_pOwnerWindow;
  
  DescriptorSetLayoutBuilder m_oMaterialLayoutBuilder;  
};

}
}
