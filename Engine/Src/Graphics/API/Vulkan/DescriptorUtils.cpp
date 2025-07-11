#include "Graphics/API/Vulkan/DescriptorUtils.h"

#include "Core/Exception.h"

#include "Graphics/API/Vulkan/VulkanMacros.h"

#include "Graphics/API/Vulkan/APIWindow.h"
#include "Graphics/API/Vulkan/APITexture.h"
#include "Graphics/API/Vulkan/APIRenderState.h"
#include "Graphics/API/Vulkan/APIInternal.h"
#include "Graphics/API/Vulkan/APIWindow.h"

#include <array>

namespace api
{
namespace vk
{

// DescriptorSetLayoutBuilder

void DescriptorSetLayoutBuilder::AddLayoutBinding(const char* _sName, VkDescriptorSetLayoutBinding&& _oBinding)
{
  /*
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
  */

  // Match not found, add entry
  m_lstDescriptors.push_back(std::move(_oBinding));
  m_lstNames.emplace_back(_sName);
}

VkDescriptorSetLayout DescriptorSetLayoutBuilder::Build(VkDevice _hDevice)
{

  VkDescriptorSetLayoutCreateInfo oDescSetLayoutInfo{};
  oDescSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  oDescSetLayoutInfo.bindingCount = m_lstDescriptors.size();
  oDescSetLayoutInfo.pBindings = m_lstDescriptors.data();

  VkDescriptorSetLayout hDescSetLayout;

  VK_CHECK(vkCreateDescriptorSetLayout(_hDevice, &oDescSetLayoutInfo, NULL, &hDescSetLayout))

    return hDescSetLayout;
}

bool DescriptorSetLayoutBuilder::Contains(VkDescriptorSetLayoutBinding _oBinding) const
{
  for (const VkDescriptorSetLayoutBinding& rBind : m_lstDescriptors)
  {
    if (rBind.binding == _oBinding.binding
      && rBind.descriptorCount == _oBinding.descriptorCount
      && rBind.descriptorType == _oBinding.descriptorType
      && rBind.stageFlags == _oBinding.stageFlags)
    {
      return true;
    }
  }
  return false;
}

uint32_t DescriptorSetLayoutBuilder::FindBinding(const std::string& _sName, VkShaderStageFlags _uStageFlags, VkDescriptorType _eType) const
{
  for (int i = 0; i < m_lstNames.size(); i++)
  {
    if (_sName == m_lstNames[i] 
      && (m_lstDescriptors[i].stageFlags & _uStageFlags) != 0u
      && m_lstDescriptors[i].descriptorType == _eType)
    {
      return m_lstDescriptors[i].binding;      
    }
  }
  return 0xFFFFFFFFu;
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

void DescriptorSetUpdater::AddBufferInfo(VkDescriptorBufferInfo&& _oBufferInfo, uint32_t _uBinding, uint32_t _uSetIdx, PipelineStageFlags _uStageFlags)
{
  for (SetBoundBufferInfoList& rInfoList : m_lstSetBufferInfos)
  {
    if (rInfoList.m_uSetIdx == _uSetIdx
      && rInfoList.m_uBinding == _uBinding
      && rInfoList.m_uStageFlags == _uStageFlags)
    {
      rInfoList.m_lstBufferInfos.push_back(std::move(_oBufferInfo));      
      return;
    }
  }

  // No match found. Create entry  

  SetBoundBufferInfoList oInfoList{};
  oInfoList.m_uSetIdx = _uSetIdx;
  oInfoList.m_uBinding = _uBinding;
  oInfoList.m_uStageFlags = _uStageFlags;
  oInfoList.m_lstBufferInfos.push_back(std::move(_oBufferInfo));
  m_lstSetBufferInfos.push_back(std::move(oInfoList));
}

void DescriptorSetUpdater::AddImageInfo(VkDescriptorImageInfo&& _oImageInfo, uint32_t _uBinding, uint32_t _uSetIdx, PipelineStageFlags _uStageFlags)
{
  for (SetBoundImgInfoList& rInfoList : m_lstSetImageInfos)
  {
    if (rInfoList.m_uSetIdx == _uSetIdx
      && rInfoList.m_uBinding == _uBinding
      && rInfoList.m_uStageFlags == _uStageFlags)
    {      
      rInfoList.m_lstImgInfos.push_back(std::move(_oImageInfo));      
      return;
    }
  }

  // No match found. Create entry  

  SetBoundImgInfoList oInfoList{};
  oInfoList.m_uSetIdx = _uSetIdx;
  oInfoList.m_uBinding = _uBinding;
  oInfoList.m_uStageFlags = _uStageFlags;
  oInfoList.m_lstImgInfos.push_back(std::move(_oImageInfo));
  m_lstSetImageInfos.push_back(std::move(oInfoList));
}

void DescriptorSetUpdater::Update(const APIWindow* _pWindow)
{    

  std::vector<VkWriteDescriptorSet> lstWriteDescSets;
  
  // CBuffer descriptors
  for (const SetBoundBufferInfoList& lstSetBufferInfo : m_lstSetBufferInfos)
  { 
    VkDescriptorSetLayoutBinding oBinding{};
    oBinding.binding = lstSetBufferInfo.m_uBinding;
    oBinding.descriptorCount = lstSetBufferInfo.m_lstBufferInfos.size();
    oBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oBinding.stageFlags = GetVkShaderFlags(lstSetBufferInfo.m_uStageFlags);
    if (m_pLayoutBuilder != nullptr && !m_pLayoutBuilder->Contains(oBinding))
    {
      continue;
    }

    if (lstSetBufferInfo.m_uSetIdx > _pWindow->m_uSwapchainImageCount)
    {
      THROW_GENERIC_EXCEPTION("WTF?")
    }

    VkWriteDescriptorSet oUboWriteDescSet{};
    oUboWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    oUboWriteDescSet.dstSet = m_pDescSets[lstSetBufferInfo.m_uSetIdx];
    oUboWriteDescSet.dstBinding = lstSetBufferInfo.m_uBinding;
    oUboWriteDescSet.dstArrayElement = 0;
    oUboWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    oUboWriteDescSet.descriptorCount = lstSetBufferInfo.m_lstBufferInfos.size();
    oUboWriteDescSet.pBufferInfo = lstSetBufferInfo.m_lstBufferInfos.data();
    oUboWriteDescSet.pImageInfo = NULL;
    oUboWriteDescSet.pTexelBufferView = NULL;

    lstWriteDescSets.push_back(std::move(oUboWriteDescSet));    
  }  
     
  // Texture descriptors
  for (const SetBoundImgInfoList& lstSetImageInfo : m_lstSetImageInfos)
  {      

    VkDescriptorSetLayoutBinding oBinding{};
    oBinding.binding = lstSetImageInfo.m_uBinding;
    oBinding.descriptorCount = lstSetImageInfo.m_lstImgInfos.size();
    oBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    oBinding.stageFlags = GetVkShaderFlags(lstSetImageInfo.m_uStageFlags);
    if (m_pLayoutBuilder != nullptr && !m_pLayoutBuilder->Contains(oBinding))
    {
      continue;
    }

    if (lstSetImageInfo.m_uSetIdx > _pWindow->m_uSwapchainImageCount)
    {
      THROW_GENERIC_EXCEPTION("WTF?")
    }

    VkWriteDescriptorSet oImageWriteDescSet{};
    oImageWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    oImageWriteDescSet.dstSet = m_pDescSets[lstSetImageInfo.m_uSetIdx];
    oImageWriteDescSet.dstBinding = lstSetImageInfo.m_uBinding;
    oImageWriteDescSet.dstArrayElement = 0;
    oImageWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    oImageWriteDescSet.descriptorCount = lstSetImageInfo.m_lstImgInfos.size();
    oImageWriteDescSet.pBufferInfo = NULL;
    oImageWriteDescSet.pImageInfo = lstSetImageInfo.m_lstImgInfos.data();

    lstWriteDescSets.push_back(std::move(oImageWriteDescSet));
  }

  // Check for missing image descriptors

  std::vector<VkDescriptorImageInfo> lstDummyImageInfos;
  lstDummyImageInfos.reserve(m_pLayoutBuilder->GetDescriptors().size() * _pWindow->m_uSwapchainImageCount);

  if (_pWindow != nullptr)
  {
    for (const VkDescriptorSetLayoutBinding& rBinding : m_pLayoutBuilder->GetDescriptors())
    {
      if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == rBinding.descriptorType)
      {
        bool bFound = false;
        for (const SetBoundImgInfoList& lstSetImageInfo : m_lstSetImageInfos)
        {
          if (lstSetImageInfo.m_uBinding == rBinding.binding
            && lstSetImageInfo.m_lstImgInfos.size() == rBinding.descriptorCount
            && GetVkShaderFlags(lstSetImageInfo.m_uStageFlags) == rBinding.stageFlags)
          {
            bFound = true;
            break;
          }
        }

        // Add dummy texture if necessary (once per swapchain image)
        if (!bFound)
        {
          for (uint32_t i = 0u; i < _pWindow->m_uSwapchainImageCount; i++)
          {
            if (!s_oGlobalData.m_bRenderBegan || i == _pWindow->m_uCurrFrameIdx)
            {
              auto& oInfo = lstDummyImageInfos.emplace_back();
              oInfo.imageView = _pWindow->m_pDummyTexture->m_hImageView;
              oInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
              oInfo.sampler = _pWindow->m_pDummyTexture->m_hSampler;

              VkWriteDescriptorSet oImageWriteDescSet{};
              oImageWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              oImageWriteDescSet.dstSet = m_pDescSets[i];
              oImageWriteDescSet.dstBinding = rBinding.binding;
              oImageWriteDescSet.dstArrayElement = 0;
              oImageWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
              oImageWriteDescSet.descriptorCount = rBinding.descriptorCount;
              oImageWriteDescSet.pBufferInfo = NULL;
              oImageWriteDescSet.pImageInfo = &oInfo;

              lstWriteDescSets.push_back(std::move(oImageWriteDescSet));
            }
          }
        }
      }
    }
  }

  if (!lstWriteDescSets.empty())
  {
    vkUpdateDescriptorSets(_pWindow->m_hDevice, lstWriteDescSets.size(), lstWriteDescSets.data(), 0, nullptr);
  }
}

}
}
