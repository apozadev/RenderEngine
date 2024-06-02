#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace api
{
namespace vk
{

  class DescriptorSetLayoutBuilder
  {
  public:

    void AddLayoutBinding(VkDescriptorSetLayoutBinding&& _rBinding);

    VkDescriptorSetLayout Build(VkDevice _hDevice);

    void Clear() { m_lstDescSetInfos.clear();}

  private:

    std::vector<VkDescriptorSetLayoutBinding> m_lstDescSetInfos;    
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
      std::vector<VkDescriptorBufferInfo> m_lstBufferInfos;
    };

    struct SetBoundImgInfoList
    {
      uint32_t m_uSetIdx;
      uint32_t m_uBinding;
      std::vector<VkDescriptorImageInfo> m_lstImgInfos;
    };

  public:

    void AddBufferInfo(VkDescriptorBufferInfo&& _oBufferInfo, uint32_t _uBinding, uint32_t _uSetIdx);
    void AddImageInfo(VkDescriptorImageInfo&& _oImageInfo, uint32_t _uBinding, uint32_t _uSetIdx);

    void Update(VkDevice _hDevice, VkDescriptorSet* _pDescSets, uint32_t _uCount);

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