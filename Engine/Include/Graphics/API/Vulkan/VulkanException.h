#include "Core/Exception.h"

#include <vector>
#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

namespace api
{
namespace vk
{
	class VulkanException : public core::Exception
	{
	public:
		VulkanException(int _sLine, const char* _sFile, VkResult _hResult) noexcept;
		const char* GetType() const noexcept override;
	};
}
}
