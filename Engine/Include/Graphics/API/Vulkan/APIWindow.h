#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk 
{
  
  struct APIWindow
  {  
    VkDevice m_hDevice;

    VkCommandPool m_hRenderCmdPool;
    VkCommandPool m_hPresentCmdPool;

    VkCommandBuffer m_hRenderCmdBuffer;
    VkCommandBuffer m_hPresentCmdBuffer;

    VkQueue m_hRenderQueue;
    VkQueue m_hPresentQueue;

    VkSurfaceKHR m_hSurface;

    VkSwapchainKHR m_hSwapchain;    
    VkImage* m_pSwapchainImages;
    VkImageView* m_pSwapChainImageViews;
    VkFramebuffer* m_pFramebuffers;

    VkImage m_hColorImage;
    VkDeviceMemory m_hColorImageMemory;
    VkImageView m_hColorImageView;

    VkImage m_hDepthImage;
    VkDeviceMemory m_hDepthImageMemory;
    VkImageView m_hDepthImageView;

    VkExtent2D m_oExtent;    

    VkFormat m_eSwapchainFormat;
    VkColorSpaceKHR m_eSwapchainColorSpace;    

    VkRenderPass m_hRenderPass;

    VkSemaphore m_hImageAvailableSemaphore;
    VkSemaphore m_hRenderFinishedSemaphore;

    VkFence m_hInFlightFence;

    uint32_t m_uSwapchainImageCount;

    uint32_t m_uCurrSwapchainImageIdx;    

    VkDescriptorPool m_hDescPool;

    VkDescriptorSetLayout m_hGlobalDescSetLayout;    

    bool m_bResized = false;
  };

}
}