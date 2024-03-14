#pragma once

#include <vulkan/vulkan.h>

namespace api
{
namespace vk {

  
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
    VkPipelineLayout m_hPipelineLayout;
    VkRenderPass m_hRenderPass;
    VkPipeline m_hGraphicsPipeline;
    VkImage* m_pSwapchainImages;
    VkImageView* m_pSwapChainImageViews;
    VkFramebuffer* m_pFramebuffers;
    VkExtent2D m_oExtent;    
    VkFormat m_eSwapchainFormat;
    VkColorSpaceKHR m_eSwapchainColorSpace;

    VkSemaphore m_hImageAvailableSemaphore;
    VkSemaphore m_hRenderFinishedSemaphore;
    VkFence m_hInFlightFence;

    uint32_t m_uSwapchainImageCount;

    uint32_t m_uNextSwapchainImageIdx;

    bool m_bResized = false;
  };

}
}