#pragma once

#if _DEBUG

#include "Graphics/API/Vulkan/VulkanException.h"

#define VK_CHECK(expr) { VkResult hResult = expr; if(hResult != VK_SUCCESS){ throw api::vk::VulkanException(__LINE__,__FILE__, hResult);}}

#else

#define VK_CHECK(expr) expr;

#endif