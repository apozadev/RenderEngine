#include "Graphics/API/Vulkan/VulkanShaderReflection.h"
#include "Graphics/API/Vulkan/DescriptorUtils.h"
#include "Graphics/API/Vulkan/VulkanPools.h"

#include "Graphics/ResourceFrequency.h"

#include "Core/Exception.h"

#include "../3rd/spirv-reflect/spirv_reflect.h"

#include <string>

#define CHECK_REFLECT(x, msg) \
if (!(x)) \
{ \
  std::string sErr(msg);  \
  sErr += _oShaderFile.GetFilename(); \
  THROW_GENERIC_EXCEPTION(sErr.c_str()) \
}

namespace api
{
namespace vk
{

void ReflectSetLayouts(const file::File& _oShaderFile, DescriptorSetLayoutBuilder& oLayoutBuilder_)
{
  SpvReflectShaderModule oModule;
  SpvReflectResult eResult = spvReflectCreateShaderModule(_oShaderFile.GetSize(), static_cast<void*>(_oShaderFile.GetData()), &oModule);
  CHECK_REFLECT(eResult == SPV_REFLECT_RESULT_SUCCESS, "[API] Error: Unable to load reflection from shader: ")
   
  const SpvReflectEntryPoint* pEntryPoint = spvReflectGetEntryPoint(&oModule, "main");

  CHECK_REFLECT(pEntryPoint != nullptr, "[API] Error: Unable to find shader entry point: ")
  
  uint32_t uDescSetCount = 0;
  eResult = spvReflectEnumerateEntryPointDescriptorSets( &oModule, "main", &uDescSetCount, NULL);
  CHECK_REFLECT(eResult == SPV_REFLECT_RESULT_SUCCESS, "[API] Error: Failed to enumerate descriptor sets from shader: ")
  CHECK_REFLECT(uDescSetCount <= 4u, "[API] Error: the shader has more than 4 descriptor sets : ")

  SpvReflectDescriptorSet* aDescSets[4];// = new SpvReflectDescriptorSet * [uDescSetCount];

  eResult = spvReflectEnumerateEntryPointDescriptorSets(&oModule, "main", &uDescSetCount, aDescSets);
  CHECK_REFLECT(eResult == SPV_REFLECT_RESULT_SUCCESS, "[API] Error: Failed to get descriptor sets from shader: ")  

  for (uint32_t i = 0u; i < uDescSetCount; i++)
  {
    SpvReflectDescriptorSet* pDescSet = aDescSets[i];

    DescriptorSetLayoutBuilder* pLayoutBuilder = nullptr;
    
    if (pDescSet->set == static_cast<uint32_t>(ResourceFrequency::MATERIAL))
    {
      for (uint32_t j = 0u; j < pDescSet->binding_count; j++)
      {
        SpvReflectDescriptorBinding* pDesc = pDescSet->bindings[j];
        VkDescriptorSetLayoutBinding oBinding = {};
        oBinding.binding = pDesc->binding;
        oBinding.descriptorType = static_cast<VkDescriptorType>(pDesc->descriptor_type);
        oBinding.descriptorCount = 1;
        oBinding.stageFlags = static_cast<VkShaderStageFlagBits>(oModule.shader_stage);
        oBinding.pImmutableSamplers = NULL;        
        pLayoutBuilder->AddLayoutBinding(pDesc->name, std::move(oBinding));
      }
    }
  }

  spvReflectDestroyShaderModule(&oModule);

}

}
}