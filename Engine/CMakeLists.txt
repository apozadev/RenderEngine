cmake_minimum_required(VERSION 3.8)

project(Engine VERSION 0.0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME}
	"Src/Graphics/Window.cpp"
	"Include/Graphics/Window.h"	
	"Include/Core/Engine.h"
	"Src/Core/Engine.cpp" 
	 
	"Include/Core/Singleton.h" 
	
	"Include/Core/Timer.h" "Src/Core/Timer.cpp" 
	"Include/Graphics/Renderer.h" 
	"Include/Graphics/API/Vulkan/VulkanAPI.h"
	"Src/Graphics/API/Vulkan/VulkanAPI.cpp" 
	"Include/Graphics/API/Vulkan/VulkanMacros.h" 
	"Include/Core/Exception.h" 
	"Src/Core/Exception.cpp" 
	"Include/Graphics/API/Vulkan/VulkanException.h" 
	"Src/Graphics/API/Vulkan/VulkanException.cpp" 
	"Include/Graphics/API/Vulkan/VulkanData.h" 
	"Src/Graphics/API/GraphicsAPI.cpp" 
	"Src/Graphics/Renderer.cpp" 
	"Include/Graphics/API/Vulkan/APIWindow.h"  "Include/File/FileUtils.h" "Src/File/FileUtils.cpp"  "Include/File/File.h" "Src/File/File.cpp" "Include/Core/GlobalMacros.h" "Include/Math/Vector3.h" "Include/Math/Vector2.h" "Include/Graphics/Mesh.h"  "Src/Graphics/Mesh.cpp" "Include/Graphics/API/Vulkan/APIMesh.h" "Include/Graphics/Vertex.h" "Include/Core/InputManager.h" "Src/Core/InputManager.cpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/Include"
	"$ENV{VULKAN_SDK}/Include"
	"${CMAKE_SOURCE_DIR}/glfw/include"
 )

 set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17)

 target_compile_definitions(${PROJECT_NAME} PRIVATE 
 RENDER_API=vk
 CURR_WORKING_DIR=${CMAKE_BINARY_DIR})