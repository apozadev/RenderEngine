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
 "Include/Core/Timer.h" "Src/Core/Timer.cpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/Include"
	"$ENV{VULKAN_SDK}/Include"
	"${CMAKE_SOURCE_DIR}/glfw/include"
 )