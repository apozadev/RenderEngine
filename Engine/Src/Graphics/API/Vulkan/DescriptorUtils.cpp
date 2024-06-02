#include "Graphics/API/Vulkan/DescriptorUtils.h"

#include "Graphics/API/Vulkan/VulkanMacros.h"

#include "Graphics/API/Vulkan/APIWindow.h"
#include "Graphics/API/Vulkan/APIRenderState.h"

#include <array>

namespace api
{
namespace vk
{

// DescriptorSetLayoutBuilder

void DescriptorSetLayoutBuilder::AddLayoutBinding(VkDescriptorSetLayoutBinding&& _rBinding)
{
  for (int i = 0; i < m_lstDescSetInfos.size(); i++)
  {
    VkDescriptorSetLayoutBinding& rOther = m_lstDescSetInfos[i];
    // If new layout binding matches present one, just update count      
    if (_rBinding.binding == rOther.binding
      && _rBinding.descriptorType == rOther.descriptorType
      && _rBinding.pImmutableSamplers == rOther.pImmutableSamplers
      && _rBinding.stageFlags == rOther.stageFlags)
    {
      _rBinding.descriptorCount++;
      return;
    }
  }

  // Match not found, add entry
  m_lstDescSetInfos.push_back(std::move(_rBinding));  
}

VkDescriptorSetLayout DescriptorSetLayoutBuilder::Build(VkDevice _hDevice)
{

  VkDescriptorSetLayoutCreateInfo oDescSetLayoutInfo{};
  oDescSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  oDescSetLayoutInfo.bindingCount = m_lstDescSetInfos.size();
  oDescSetLayoutInfo.pBindings = m_lstDescSetInfos.data();

  VkDescriptorSetLayout hDescSetLayout;

  VK_CHECK(vkCreateDescriptorSetLayout(_hDevice, &oDescSetLayoutInfo, NULL, &hDescSetLayout))

    return hDescSetLayout;
}

// DescriptorPoolBuilder

void DescriptorPoolBuilder::AddPoolSize(VkDescriptorPoolSize&& _rPoolSize)
{

  _rPoolSize.descriptorCount *= m_uSwapchainSize; 

  for (VkDescriptorPoolSize& rPoolSize : m_lstPoolSizes)
  {
    if (rPoolSize.type == _rPoolSize.type)
    {
      rPoolSize.descriptorCount += _rPoolSize.descriptorCount;
      return;
    }
  }

  // No match found. Create new entry
  m_lstPoolSizes.push_back(std::move(_rPoolSize));
}

VkDescriptorPool DescriptorPoolBuilder::Build(VkDevice _hDevice)
{
  VkDescriptorPoolCreateInfo oDescPoolInfo{};
  oDescPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  oDescPoolInfo.flags = 0;
  oDescPoolInfo.poolSizeCount = m_lstPoolSizes.size();
  oDescPoolInfo.pPoolSizes = m_lstPoolSizes.data();
  oDescPoolInfo.maxSets = m_uSwapchainSize;

  VkDescriptorPool hDescPool;
  VK_CHECK(vkCreateDescriptorPool(_hDevice, &oDescPoolInfo, NULL, &hDescPool));

  return hDescPool;
}

void DescriptorSetUpdater::AddBufferInfo(VkDescriptorBufferInfo&& _oBufferInfo, uint32_t _uBinding, uint32_t _uSetIdx)
{
  for (SetBoundBufferInfoList& rInfoList : m_lstSetBufferInfos)
  {
    if (rInfoList.m_uSetIdx == _uSetIdx
      && rInfoList.m_uBinding == _uBinding)
    {
      rInfoList.m_lstBufferInfos.push_back(std::move(_oBufferInfo));      
      return;
    }
  }

  // No match found. Create entry  

  SetBoundBufferInfoList oInfoList{};
  oInfoList.m_uSetIdx = _uSetIdx;
  oInfoList.m_uBinding = _uBinding;
  oInfoList.m_lstBufferInfos.push_back(std::move(_oBufferInfo));
  m_lstSetBufferInfos.push_back(std::move(oInfoList));
}

void DescriptorSetUpdater::AddImageInfo(VkDescriptorImageInfo&& _oImageInfo, uint32_t _uBinding, uint32_t _uSetIdx)
{
  for (SetBoundImgInfoList& rInfoList : m_lstSetImageInfos)
  {
    if (rInfoList.m_uSetIdx == _uSetIdx
      && rInfoList.m_uBinding == _uBinding)
    {      
      rInfoList.m_lstImgInfos.push_back(std::move(_oImageInfo));      
      return;
    }
  }

  // No match found. Create entry  

  SetBoundImgInfoList oInfoList{};
  oInfoList.m_uSetIdx = _uSetIdx;
  oInfoList.m_uBinding = _uBinding;
  oInfoList.m_lstImgInfos.push_back(std::move(_oImageInfo));
  m_lstSetImageInfos.push_back(std::move(oInfoList));
}

void DescriptorSetUpdater::Update(VkDevice _hDevice, VkDescriptorSet* _pDescSets, uint32_t _uCount)
{  

  //if (m_lstSetBufferInfos.size() < _uCount || m_lstSetImageInfos.size() < _uCount)
  //{
  //  THROW_GENERIC_EXCEPTION("[API] Trying to update too many descriptor sets")
  //}

  bool bHasUboDesc = !m_lstSetBufferInfos.empty();
  bool bHasImgDesc = !m_lstSetImageInfos.empty();

  unsigned int uDescCount = 0u;

  if (bHasUboDesc)
  {
    uDescCount += _uCount;
  } 
  if (bHasImgDesc)
  {
    uDescCount += _uCount;
  }

  std::vector<VkWriteDescriptorSet> lstWriteDescSets(uDescCount);

  uint32_t uCurrIdx = 0u;

  if (bHasUboDesc)
  {
    for (uint32_t i = 0; i < _uCount; i++)
    {
      std::vector<VkDescriptorBufferInfo>& lstBufferInfos = m_lstSetBufferInfos[i].m_lstBufferInfos;
      VkWriteDescriptorSet oUboWriteDescSet{};
      oUboWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      oUboWriteDescSet.dstSet = _pDescSets[i];
      oUboWriteDescSet.dstBinding = m_lstSetBufferInfos[i].m_uBinding;
      oUboWriteDescSet.dstArrayElement = 0;
      oUboWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      oUboWriteDescSet.descriptorCount = lstBufferInfos.size();
      oUboWriteDescSet.pBufferInfo = lstBufferInfos.size() ? lstBufferInfos.data() : NULL;
      oUboWriteDescSet.pImageInfo = NULL;
      oUboWriteDescSet.pTexelBufferView = NULL;

      lstWriteDescSets[uCurrIdx++] = std::move(oUboWriteDescSet);
    }
  }
   
  if (bHasImgDesc)
  {
    for (uint32_t i = 0; i < _uCount; i++)
    {
      std::vector<VkDescriptorImageInfo>& lstImgInfos = m_lstSetImageInfos[i].m_lstImgInfos;
      VkWriteDescriptorSet oImageWriteDescSet{};
      oImageWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      oImageWriteDescSet.dstSet = _pDescSets[i];
      oImageWriteDescSet.dstBinding = m_lstSetImageInfos[i].m_uBinding;
      oImageWriteDescSet.dstArrayElement = 0;
      oImageWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      oImageWriteDescSet.descriptorCount = lstImgInfos.size();
      oImageWriteDescSet.pBufferInfo = NULL;
      oImageWriteDescSet.pImageInfo = lstImgInfos.size() ? lstImgInfos.data() : NULL;
      oImageWriteDescSet.pTexelBufferView = NULL;

      lstWriteDescSets[uCurrIdx++] = std::move(oImageWriteDescSet);
    }
  }

  if (bHasImgDesc || bHasUboDesc)
  {
    vkUpdateDescriptorSets(_hDevice, lstWriteDescSets.size(), lstWriteDescSets.data(), 0, nullptr);
  }
}

}
}
