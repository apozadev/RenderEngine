#pragma once

#include <vulkan/vulkan.h>

#include "../3rd/spirv-reflect/spirv_reflect.h"

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

  SpvReflectShaderModule m_oVertexReflection;
  SpvReflectShaderModule m_oGeomReflection;
  SpvReflectShaderModule m_oPixelReflection;
};

}
}
