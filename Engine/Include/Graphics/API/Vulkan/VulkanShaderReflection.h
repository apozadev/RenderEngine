#pragma once

#include "File/File.h"
#include "Graphics/PipelineStage.h"
#include "Graphics/API/Vulkan/DescriptorUtils.h"

namespace api
{
namespace vk
{

  void ReflectSetLayouts(const file::File& _oShaderFile, DescriptorSetLayoutBuilder& oMatLayoutBuilder_);

}
}