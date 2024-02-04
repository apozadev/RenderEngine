cmake_minimum_required(VERSION 3.8)

project(Engine VERSION 0.0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME}
	"Src/Core/Window.cpp"
	"Include/Core/Window.h"	
	"Include/Core/Engine.h"
	"Src/Core/Engine.cpp" 
	"Include/Core/WindowManager.h" 
	"Include/Core/Singleton.h" 
	"Src/Core/WindowManager.cpp"
	 "Include/Core/Timer.h" "Src/Core/Timer.cpp" 
	 "Include/Graphics/Renderer.h" 
	 "Include/Graphics/API/Vulkan/VulkanAPI.h"
	 "Src/Graphics/API/Vulkan/VulkanAPI.cpp" "Include/Graphics/API/Vulkan/VulkanMacros.h" "Include/Core/Exception.h" "Src/Core/Exception.cpp" "Include/Graphics/API/Vulkan/VulkanException.h" "Src/Graphics/API/Vulkan/VulkanException.cpp" "Include/Graphics/API/Vulkan/VulkanData.h" "Src/Graphics/API/GraphicsAPI.cpp" "Src/Graphics/Renderer.cpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/Include"
	"$ENV{VULKAN_SDK}/Include"
	"${CMAKE_SOURCE_DIR}/glfw/include"
 )

 target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API=vk)