cmake_minimum_required(VERSION 3.8)

project(Editor VERSION 0.0.1 LANGUAGES CXX)

set(
	source_list
	"main.cpp"
    "SceneEditor.h" 
    "SceneEditor.cpp"
    "../imgui/imgui.h"
    "../imgui/imgui.cpp"
    "../imgui/imgui_internal.h"    
    "../imgui/imgui_draw.cpp"    
    "../imgui/imgui_tables.cpp"    
    "../imgui/imgui_widgets.cpp"    
    "../imgui/imstb_rectpack.h"    
    "../imgui/imstb_textedit.h"    
    "../imgui/imstb_truetype.h"    
    "../imgui/backends/imgui_impl_vulkan.h"
    "../imgui/backends/imgui_impl_vulkan.cpp"
    "../imgui/backends/imgui_impl_win32.h"
    "../imgui/backends/imgui_impl_win32.cpp"
    "../imgui/backends/imgui_impl_glfw.h"
    "../imgui/backends/imgui_impl_glfw.cpp")

add_executable(${PROJECT_NAME} ${source_list})

foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/Engine/Include ${CMAKE_SOURCE_DIR}/Editor ${CMAKE_SOURCE_DIR}/Editor/3rd)

link_directories($ENV{VULKAN_SDK}/Bin;$ENV{VULKAN_SDK}/Lib;)

find_package(Vulkan REQUIRED)
target_link_libraries(Editor Vulkan::Vulkan)
target_link_libraries(Editor d3d11.lib)
target_link_libraries(Editor dxgi.lib)
target_link_libraries(Editor d3dcompiler.lib)
target_link_libraries(Editor dxguid.lib)
target_link_libraries(Editor Engine)
target_link_libraries(Editor glfw ${GLFW_LIBRARIES})
target_link_libraries(Editor glm::glm)
target_link_libraries(Editor assimp::assimp)

if(${TARGET_RENDER_API} MATCHES ${RENDER_API_VULKAN})
	target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API_VULKAN) 
else()
target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API_DX11) 
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17
    # VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
