#include "Graphics\API\Vulkan\VulkanException.h"

#include <vulkan/vk_enum_string_helper.h>

#include <sstream>

namespace api
{
namespace vk
{

	VulkanException::VulkanException(int _sLine, const char* _sFile, VkResult _hResult) noexcept
		: Exception(_sLine, _sFile, "")
	{				
		m_sDescription = string_VkResult(_hResult);
	}	

	const char* VulkanException::GetType() const noexcept
	{
		return "Vulkan Exception";
	}

}
}
