#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "Graphics/PipelineStage.h"

namespace api
{
namespace vk
{

  class DescriptorSetLayoutBuilder
  {

    struct Descriptor
    {
      std::string m_sName;
      VkDescriptorSetLayoutBinding m_oBinding;
    };

  public:

    void AddLayoutBinding(const char* _sName, VkDescriptorSetLayoutBinding&& _oBinding);

    VkDescriptorSetLayout Build(VkDevice _hDevice);    

    bool Contains(VkDescriptorSetLayoutBinding _oBinding) const;

    uint32_t FindBinding(const std::string& _sName, PipelineStage _eStage, VkDescriptorType _eType) const;

    void Clear() { m_lstDescriptors.clear();}

  private:

    std::vector<VkDescriptorSetLayoutBinding> m_lstDescriptors;
    std::vector<std::string> m_lstNames;
  };

  //------------------------------------------------------

  class DescriptorPoolBuilder
  {
  public:

    void SetSwapchainSize(unsigned int _uSize) { m_uSwapchainSize = _uSize; }

    void AddPoolSize(VkDescriptorPoolSize&& _rPoolSize);

    VkDescriptorPool Build(VkDevice _hDevice);

    void Clear() { m_lstPoolSizes.clear(); m_uSwapchainSize = 1u; }

  private:

    std::vector<VkDescriptorPoolSize> m_lstPoolSizes;

    unsigned int m_uSwapchainSize = 1u;
  };

  //------------------------------------------------------

  class DescriptorSetUpdater
  {

  private:

    struct SetBoundBufferInfoList
    {
      uint32_t m_uSetIdx;      
      uint32_t m_uBinding;
      PipelineStage m_eStage;
      std::vector<VkDescriptorBufferInfo> m_lstBufferInfos;
    };

    struct SetBoundImgInfoList
    {
      uint32_t m_uSetIdx;
      uint32_t m_uBinding;
      PipelineStage m_eStage;
      std::vector<VkDescriptorImageInfo> m_lstImgInfos;
    };

  public:

    void AddBufferInfo(VkDescriptorBufferInfo&& _oBufferInfo, uint32_t _uBinding, uint32_t _uSetIdx, PipelineStage _eStage);
    void AddImageInfo(VkDescriptorImageInfo&& _oImageInfo, uint32_t _uBinding, uint32_t _uSetIdx, PipelineStage _eStage);

    void Update(VkDevice _hDevice, VkDescriptorSet* _pDescSets, uint32_t _uCount, DescriptorSetLayoutBuilder* _pLayoutBuilder);

    void Clear() 
    { 
      m_lstSetBufferInfos.clear(); 
      m_lstSetImageInfos.clear();            
    }

  private:

    std::vector<SetBoundBufferInfoList> m_lstSetBufferInfos;
    std::vector<SetBoundImgInfoList> m_lstSetImageInfos;

  };

}
}