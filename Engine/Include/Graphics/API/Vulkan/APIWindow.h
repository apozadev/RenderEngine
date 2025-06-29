#pragma once

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace api
{
namespace vk 
{

  struct APITexture;
  
  struct APIWindow
  {  
    VkDevice m_hDevice;

    VkCommandPool m_hCmdPool;

    VkCommandBuffer* m_pCmdBuffers;

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

    VkSemaphore* m_pImageAvailableSemaphores;
    VkSemaphore* m_pRenderFinishedSemaphores;

    VkFence* m_pInFlightFences;

    uint32_t m_uSwapchainImageCount;

    uint32_t m_uCurrSwapchainImageIdx;    

    uint32_t m_uCurrFrameIdx;    

    VkDescriptorPool m_hDescPool;

    VkDescriptorSetLayout m_hGlobalDescSetLayout;    
    VkDescriptorSetLayout m_hStepDescSetLayout;    
    VkDescriptorSetLayout m_hMatInstanceDescSetLayout;    

    DescriptorSetLayoutBuilder m_oGlobalLayoutBuilder;
    DescriptorSetLayoutBuilder m_oStepLayoutBuilder;
    DescriptorSetLayoutBuilder m_oMatInstanceLayoutBuilder;

    APITexture* m_pDummyTexture;

    bool m_bResized = false;

    VkSampleCountFlagBits m_eMsaaSamples;

    GLFWwindow* m_pGlfwWindow;
  };

}
}