#pragma once

#include "File/File.h"
#include "Graphics/PipelineStage.h"
#include "Graphics/API/Vulkan/DescriptorUtils.h"
#include "Graphics/API/Vulkan/APIRenderState.h"

namespace api
{
namespace vk
{

  void ReflectSetLayouts(const SpvReflectShaderModule& _oReflection, DescriptorSetLayoutBuilder& rLayoutBuilder_);

}
}