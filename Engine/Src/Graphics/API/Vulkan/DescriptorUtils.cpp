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

void DescriptorSetLayoutBuilder::AddLayoutBinding(VkDescriptorSetLayoutBinding&& _rBinding, int _iGroupId)
{
  for (int i = 0; i < m_lstDescSetInfos.size(); i++)
  {
    VkDescriptorSetLayoutBinding& rOther = m_lstDescSetInfos[i];
    // If new layout binding matches present one, just update count      
    if (m_lstGroupIds[i] == _iGroupId
      && _rBinding.binding == rOther.binding
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
  m_lstGroupIds.push_back(_iGroupId);
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

void DescriptorSetUpdater::AddBufferInfo(VkDescriptorBufferInfo&& _oBufferInfo, uint32_t _uSetIdx)
{
  for (SetBufferInfos& rSetBufferinfo : m_lstSetBufferInfos)
  {
    if (rSetBufferinfo.m_uSetIdx == _uSetIdx)
    {
      rSetBufferinfo.m_lstBufferInfos.push_back(std::move(_oBufferInfo));
      return;
    }
  }
  
  // No match found. Create entry
  SetBufferInfos oSetBufferInfo{};
  oSetBufferInfo.m_uSetIdx = _uSetIdx;
  oSetBufferInfo.m_lstBufferInfos.push_back(std::move(_oBufferInfo));
  m_lstSetBufferInfos.push_back(std::move(oSetBufferInfo));
}

void DescriptorSetUpdater::AddImageInfo(VkDescriptorImageInfo&& _oImageInfo, uint32_t _uSetIdx)
{
  for (SetImageInfos& rSetImageinfo : m_lstSetImageInfos)
  {
    if (rSetImageinfo.m_uSetIdx == _uSetIdx)
    {
      rSetImageinfo.m_lstImageInfos.push_back(std::move(_oImageInfo));
      return;
    }
  }

  // No match found. Create entry
  SetImageInfos oSetImageInfo{};
  oSetImageInfo.m_uSetIdx = _uSetIdx;
  oSetImageInfo.m_lstImageInfos.push_back(std::move(_oImageInfo));
  m_lstSetImageInfos.push_back(std::move(oSetImageInfo));
}

void DescriptorSetUpdater::Update(VkDevice _hDevice, VkDescriptorSet* _pDescSets, uint32_t _uCount)
{

  if (m_lstSetBufferInfos.size() < _uCount || m_lstSetImageInfos.size() < _uCount)
  {
    THROW_GENERIC_EXCEPTION("[API] Trying to update too many descriptor sets")
  }

  std::vector<VkWriteDescriptorSet> lstWriteDescSets(_uCount * 2);

  for (uint32_t i = 0; i < _uCount; i++)
  {    

    std::vector<VkDescriptorBufferInfo>& lstBufferInfos = m_lstSetBufferInfos[i].m_lstBufferInfos;
    VkWriteDescriptorSet oUboWriteDescSet{};
    oUboWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    oUboWriteDescSet.dstSet = _pDescSets[i];
    oUboWriteDescSet.dstBinding = 0;
    oUboWriteDescSet.dstArrayElement = 0;
    oUboWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oUboWriteDescSet.descriptorCount = lstBufferInfos.size();
    oUboWriteDescSet.pBufferInfo = lstBufferInfos.size()? lstBufferInfos.data() : NULL;
    oUboWriteDescSet.pImageInfo = NULL;
    oUboWriteDescSet.pTexelBufferView = NULL;

    lstWriteDescSets[i * 2] = std::move(oUboWriteDescSet);
    
    std::vector<VkDescriptorImageInfo>& lstImageInfos = m_lstSetImageInfos[i].m_lstImageInfos;
    VkWriteDescriptorSet oImageWriteDescSet{};
    oImageWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    oImageWriteDescSet.dstSet = _pDescSets[i];
    oImageWriteDescSet.dstBinding = 1;
    oImageWriteDescSet.dstArrayElement = 0;
    oImageWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    oImageWriteDescSet.descriptorCount = lstImageInfos.size();
    oImageWriteDescSet.pBufferInfo = NULL;
    oImageWriteDescSet.pImageInfo = lstImageInfos.size() ? lstImageInfos.data() : NULL;
    oImageWriteDescSet.pTexelBufferView = NULL;

    lstWriteDescSets[i * 2 + 1] = std::move(oImageWriteDescSet);
  }

  vkUpdateDescriptorSets(_hDevice, lstWriteDescSets.size(), lstWriteDescSets.data(), 0, nullptr);
}

}
}
